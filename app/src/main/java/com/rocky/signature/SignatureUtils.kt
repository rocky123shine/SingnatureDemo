package com.rocky.signature

import android.content.Context

/**
 * <pre>
 *     author : rocky
 *     time   : 2022/07/04
 * </pre>
 */
class SignatureUtils {


    companion object {
        init {
            System.loadLibrary("signature")
        }
        @JvmStatic
        external fun signatureParams(params_:String): String
        @JvmStatic
        external fun signatureVerify(context: Context)
    }
}