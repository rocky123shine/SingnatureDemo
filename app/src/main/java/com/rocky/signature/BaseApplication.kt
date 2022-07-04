package com.rocky.signature

import android.app.Application

/**
 * <pre>
 *     author : rocky
 *     time   : 2022/07/04
 * </pre>
 */
open class BaseApplication :Application() {

    override fun onCreate() {
        super.onCreate()
        SignatureUtils.signatureVerify(this)
    }

}