#include <jni.h>
#include <string>
#include <android/log.h>

using namespace std;
static char *EXTRA_SIGNATURE = "ROCKY";
static int is_verify = 0;
static char *PACKAGE_NAME = "com.rocky.signature";
static char *APP_SIGNATURE = "308201dd30820146020101300d06092a864886f70d010105050030373116301406035504030c0d416e64726f69642044656275673110300e060355040a0c07416e64726f6964310b3009060355040613025553301e170d3138303430323033323733355a170d3438303332353033323733355a30373116301406035504030c0d416e64726f69642044656275673110300e060355040a0c07416e64726f6964310b300906035504061302555330819f300d06092a864886f70d010101050003818d003081890281810080ca0d1ec7339c1fa27e1de91515b2d7f36bdcf7f5625dbd6adf3f573cbe13feed0d2b6fad6d3b1a2a8f51fa808ee76c1aae314f00798d1920a74750a747fbd15d5ade186022427c751cc6e42ae78735233f221d0b1c4678b027fc3599122fc6b3c7861a418c8b873b268ecbb40b93a951fb89fee0ca3dc23fe0a69b4be8bc770203010001300d06092a864886f70d0101050500038181007d16067ca58fdbea664f92638232da06657930c171b3cc73a3c3d3659726a7dd6f095bcf3b9fa07c744b7b1b533fe0bfe45e50bfbb1780452e7efc086da42e9385d03d6ebd33cad5c70bbd8f706e6e314ccc5a5c22f53ccbc96109415462c6fed5b289215a0d11392df25decdfbfe88750a144306bc31485739c7b09f73590e2";

#include "md5.h"
#define LOG(MSG) __android_log_print(ANDROID_LOG_ERROR, "JNI_TAG", "JNI_TAG---->%s", MSG);


extern "C"
JNIEXPORT jstring JNICALL
Java_com_rocky_signature_SignatureUtils_signatureParams(JNIEnv *env, jclass thiz,
                                                        jstring params_) {


    if (is_verify == 0) return env->NewStringUTF("error_signature");

    const char *params = env->GetStringUTFChars(params_, 0);

    //加盐
    string signature_str(params);
    signature_str.insert(0, EXTRA_SIGNATURE);
    //去掉两位
    signature_str.substr(0, signature_str.length() - 2);

    //加密
    MD5_CTX *ctx = new MD5_CTX();
    MD5Init(ctx);
    MD5Update(ctx, (unsigned char *) signature_str.c_str(), signature_str.length());
    unsigned char digest[16] = {0};
    MD5Final(digest, ctx);
    //三十二位处理
    char md5_str[34];
    for (int i = 0; i < 16; ++i) {
        sprintf(md5_str, "%s%02x", md5_str, digest[i]);
    }


    env->ReleaseStringUTFChars(params_, params);

    return env->NewStringUTF(md5_str);
}


extern "C"
JNIEXPORT void JNICALL
Java_com_rocky_signature_SignatureUtils_signatureVerify(JNIEnv *env, jclass thiz,
                                                        jobject context) {

    /**
     *         val packageInfo =
            packageManager.getPackageInfo(packageName, PackageManager.GET_SIGNING_CERTIFICATES)
        val apkSigners = packageInfo.signingInfo.signingCertificateHistory
        println("签名是---》${apkSigners[0].toCharsString()}")
     */
    //获取包名
    jclass j_class = env->GetObjectClass(context);
    jmethodID j_id = env->GetMethodID(j_class, "getPackageName", "()Ljava/lang/String;");
    jstring j_package_name = static_cast<jstring>(env->CallObjectMethod(context, j_id));
    //对比包名
    const char *c_package_name = env->GetStringUTFChars(j_package_name, NULL);
    if (strcmp(c_package_name, PACKAGE_NAME) != 0) {
        LOG("包名验证失败");
        return;
    }
    //__android_log_print(ANDROID_LOG_ERROR, "JNI_TAG", "包名一致：%s", PACKAGE_NAME);
    LOG("包名一致");
    //获取签名
    //  packageManager.getPackageInfo(

    jmethodID jmethodId = env->GetMethodID(j_class, "getPackageManager",
                                           "()Landroid/content/pm/PackageManager;");
    jobject packageManagerObj = env->CallObjectMethod(context, jmethodId);
    jclass pm_class = env->GetObjectClass(packageManagerObj);
    jmethodId = env->GetMethodID(pm_class, "getPackageInfo",
                                 "(Ljava/lang/String;I)Landroid/content/pm/PackageInfo;");
    jobject package_info = env->CallObjectMethod(packageManagerObj, jmethodId, j_package_name,
                                                 0x08000000);

    j_class = env->GetObjectClass(package_info);
    jfieldID jfieldId = env->GetFieldID(j_class, "signingInfo", "Landroid/content/pm/SigningInfo;");
    jobject jSigningInfo = env->GetObjectField(package_info, jfieldId);
    j_class = env->GetObjectClass(jSigningInfo);
    jmethodId = env->GetMethodID(j_class, "getSigningCertificateHistory",
                                 "()[Landroid/content/pm/Signature;");
    jobjectArray jSigningInfoArray = static_cast<jobjectArray>(env->CallObjectMethod(jSigningInfo,

                                                                                     jmethodId));
    jobject jSigning = env->GetObjectArrayElement(jSigningInfoArray, 0);

    j_class = env->GetObjectClass(jSigning);
    jmethodId = env->GetMethodID(j_class, "toCharsString", "()Ljava/lang/String;");
    jstring j_signature_str = static_cast<jstring>(env->CallObjectMethod(jSigning, jmethodId));
    const char *c_signature_str = env->GetStringUTFChars(j_signature_str, NULL);
    // 对比签名是否一直
    if (strcmp(c_signature_str, APP_SIGNATURE) != 0) {
        LOG("签名验证失败。。。。");
        return;
    }
    //认证成功
    is_verify = 1;
    LOG("签名验证成功。。。。");


    env->ReleaseStringUTFChars(j_package_name,c_package_name);


}