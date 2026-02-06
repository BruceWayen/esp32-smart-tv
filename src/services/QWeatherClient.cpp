/**
 * @file QWeatherClient.cpp
 * @brief 和风天气接口客户端实现
 * @version 1.0.0
 */

#include "QWeatherClient.h"
#include "config/Config.h"
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <mbedtls/base64.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/entropy.h>
#include <mbedtls/pk.h>
#include <memory>
#include <time.h>

QWeatherClient::QWeatherClient()
    : _jwtToken("")
    , _jwtExpireEpoch(0)
    , _ready(false) {}

bool QWeatherClient::begin() {
    // 同步NTP时间，确保JWT时间戳有效
    configTime(NTP_TIMEZONE_OFFSET_SEC, 0, NTP_SERVER);

    time_t now = time(nullptr);
    if (now < 100000) {
        DEBUG_PRINTLN("[天气] 正在同步时间...");
        for (int i = 0; i < 10; ++i) {
            delay(500);
            now = time(nullptr);
            if (now > 100000) {
                break;
            }
        }
    }

    if (now < 100000) {
        DEBUG_PRINTLN("[天气] 时间同步失败，JWT可能不可用");
    } else {
        DEBUG_PRINTLN("[天气] 时间同步完成");
    }

    _ready = true;
    return true;
}

bool QWeatherClient::fetchCityLookup(const String& locationPinyin, String& responseJson) {
    String path = String("/geo/v2/city/lookup?location=") + locationPinyin + "&range=cn&lang=zh-hans";
    int statusCode = 0;
    return sendGetRequest(path, responseJson, statusCode) && statusCode == 200;
}

bool QWeatherClient::fetchNowWeather(const String& cityId, String& responseJson) {
    String path = String("/v7/weather/now?location=") + cityId;
    int statusCode = 0;
    return sendGetRequest(path, responseJson, statusCode) && statusCode == 200;
}

bool QWeatherClient::fetchDailyWeather(const String& cityId, String& responseJson) {
    String path = String("/v7/weather/7d?location=") + cityId;
    int statusCode = 0;
    return sendGetRequest(path, responseJson, statusCode) && statusCode == 200;
}

bool QWeatherClient::ensureJwtToken() {
    time_t now = time(nullptr);
    if (!_jwtToken.isEmpty() && now > 0 && now < _jwtExpireEpoch - 60) {
        return true;
    }
    return refreshJwtToken();
}

bool QWeatherClient::refreshJwtToken() {
    time_t now = time(nullptr);
    if (now <= 0) {
        DEBUG_PRINTLN("[天气] 系统时间异常，无法生成JWT");
        return false;
    }

    // 生成JWT头和载荷
    int iat = static_cast<int>(now) - 30;
    int exp = static_cast<int>(now) + 86000;

    String header = String("{\"alg\":\"EdDSA\",\"kid\":\"") + QWEATHER_KID + "\"}";
    String payload = String("{\"iat\":") + iat + ",\"exp\":" + exp + ",\"sub\":\"" + QWEATHER_SUB + "\"}";

    String headerB64 = base64UrlEncode(reinterpret_cast<const uint8_t*>(header.c_str()), header.length());
    String payloadB64 = base64UrlEncode(reinterpret_cast<const uint8_t*>(payload.c_str()), payload.length());
    String message = headerB64 + "." + payloadB64;

    // 使用EdDSA签名JWT
    String signatureB64;
    if (!signJwtMessage(message, signatureB64)) {
        DEBUG_PRINTLN("[天气] JWT签名失败");
        return false;
    }

    _jwtToken = message + "." + signatureB64;
    _jwtExpireEpoch = exp;
    DEBUG_PRINTLN("[天气] JWT已更新");
    return true;
}

bool QWeatherClient::sendGetRequest(const String& path, String& responseJson, int& statusCode) {
    if (!_ready) {
        return false;
    }

    if (!ensureJwtToken()) {
        return false;
    }

    String url = String(QWEATHER_BASE_URL) + path;
    // HTTPS请求使用TLS连接
    WiFiClientSecure client;
    client.setInsecure();

    HTTPClient http;
    http.begin(client, url);
    http.addHeader("Authorization", String("Bearer ") + _jwtToken);
    statusCode = http.GET();

    if (statusCode == 401) {
        DEBUG_PRINTLN("[天气] JWT过期，准备重新生成");
        http.end();
        if (!refreshJwtToken()) {
            return false;
        }
        http.begin(client, url);
        http.addHeader("Authorization", String("Bearer ") + _jwtToken);
        statusCode = http.GET();
    }

    if (statusCode > 0) {
        responseJson = http.getString();
    } else {
        DEBUG_PRINTLN("[天气] HTTP请求失败");
    }

    http.end();
    return statusCode > 0;
}

String QWeatherClient::base64UrlEncode(const uint8_t* data, size_t length) {
    size_t outLen = 0;
    size_t bufferLen = ((length + 2) / 3) * 4 + 1;
    std::unique_ptr<unsigned char[]> buffer(new unsigned char[bufferLen]);
    if (mbedtls_base64_encode(buffer.get(), bufferLen, &outLen, data, length) != 0) {
        return "";
    }

    String result = String(reinterpret_cast<char*>(buffer.get())).substring(0, outLen);
    result.replace("+", "-");
    result.replace("/", "_");
    result.replace("=", "");
    return result;
}

bool QWeatherClient::signJwtMessage(const String& message, String& signatureBase64Url) {
    mbedtls_pk_context pk;
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctrDrbg;

    mbedtls_pk_init(&pk);
    mbedtls_entropy_init(&entropy);
    mbedtls_ctr_drbg_init(&ctrDrbg);

    const char* pers = "qweather";
    if (mbedtls_ctr_drbg_seed(&ctrDrbg, mbedtls_entropy_func, &entropy,
                              reinterpret_cast<const unsigned char*>(pers), strlen(pers)) != 0) {
        DEBUG_PRINTLN("[天气] 随机数初始化失败");
        mbedtls_pk_free(&pk);
        mbedtls_ctr_drbg_free(&ctrDrbg);
        mbedtls_entropy_free(&entropy);
        return false;
    }

    int ret = mbedtls_pk_parse_key(&pk,
                                   reinterpret_cast<const unsigned char*>(QWEATHER_PRIVATE_KEY_PEM),
                                   strlen(QWEATHER_PRIVATE_KEY_PEM) + 1,
                                   nullptr,
                                   0);
    if (ret != 0) {
        DEBUG_PRINTLN("[天气] 私钥解析失败");
        mbedtls_pk_free(&pk);
        mbedtls_ctr_drbg_free(&ctrDrbg);
        mbedtls_entropy_free(&entropy);
        return false;
    }

    unsigned char signature[MBEDTLS_PK_SIGNATURE_MAX_SIZE];
    size_t sigLen = 0;
    ret = mbedtls_pk_sign(&pk,
                          MBEDTLS_MD_NONE,
                          reinterpret_cast<const unsigned char*>(message.c_str()),
                          message.length(),
                          signature,
                          &sigLen,
                          mbedtls_ctr_drbg_random,
                          &ctrDrbg);
    if (ret != 0) {
        DEBUG_PRINTLN("[天气] 签名失败");
        mbedtls_pk_free(&pk);
        mbedtls_ctr_drbg_free(&ctrDrbg);
        mbedtls_entropy_free(&entropy);
        return false;
    }

    signatureBase64Url = base64UrlEncode(signature, sigLen);

    mbedtls_pk_free(&pk);
    mbedtls_ctr_drbg_free(&ctrDrbg);
    mbedtls_entropy_free(&entropy);
    return true;
}
