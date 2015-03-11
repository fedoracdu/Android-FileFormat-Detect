/*
 * Copyright (c) Christos Zoulas 2003.
 * All Rights Reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice immediately at the beginning of the file, without modification,
 *    this list of conditions, and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *  
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <android/log.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <jni.h>

#include "magic.h"

#define MSG_TAG	"libmagic"

static int init_result = 0;
static struct magic_set *ms = NULL;

jstring Java_com_tyl_filemimetype_FileMimeType_getFileMimeType(JNIEnv* env, jobject thiz, jstring fileName)
{
	const char *type;

	if (!init_result) {
		type = "";
	} else {
		type = magic_file(ms, (*env)->GetStringUTFChars(env, fileName, 0));
		if (!type)
			type = "unkown";
	}

	return (*env)->NewStringUTF(env, type);
}

jint Java_com_tyl_filemimetype_FileMimeType_initMagic(JNIEnv* env, jobject thiz, jstring magic_path)
{
    if (init_result) {
	return 0;   //已经初始化完成
    }
    
    init_result = 0;

    const char *utf8 = (*env)->GetStringUTFChars(env, magic_path, NULL);
    if (!utf8) {
	return -1;
    }

    ms = magic_open(MAGIC_NONE);
    if (!ms) {
	__android_log_print(ANDROID_LOG_WARN, MSG_TAG, "magic_open failed");
	goto fail;
    }

    if (-1 == magic_load(ms, utf8)) {
	__android_log_print(ANDROID_LOG_WARN, MSG_TAG, "magic_load failed");
	goto fail;
    }

    init_result = 1;

    (*env)->ReleaseStringUTFChars(env, magic_path, utf8);

    return 0;

fail:
    (*env)->ReleaseStringUTFChars(env, magic_path, utf8);

    return -1;
}
