package com.tyl.filemimetype;

import android.content.Context;
import android.content.res.AssetManager;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;

/**
 * Created by tyl on 15/3/10.
 * detect file format based on libmagic
 */
public class FileMimeType implements Runnable{
    private Thread          thread;
    private Context         context;
    private STATUS          status;
    private InitFinishListener  listener;

    private static FileMimeType INSTANCE;

    private native int initMagic(String magicFilePath);
    private native String getFileMimeType(String path);

    private enum STATUS {
        UN_INIT, INITING, INITED, INIT_FAIL
    }

    public interface InitFinishListener {
        public void initFinish();
    }

    private FileMimeType(Context context) {
        this.context = context;

        thread = new Thread(this);

        status = STATUS.UN_INIT;
    }

    public synchronized static FileMimeType getInstance(Context context) {
        if (null == INSTANCE) {
            INSTANCE = new FileMimeType(context);
        }

        return INSTANCE;
    }

    @Override
    public void run() {
        System.loadLibrary("filemimetype");

        String dirPath = context.getFilesDir().getPath();
        String path = dirPath + "/magic.mgc";
        boolean initOk = true;

        if (!new File(path).exists()) {
            AssetManager assetManager = context.getAssets();
            try {
                InputStream inputStream = assetManager.open("magic.mgc");
                FileOutputStream fileOutputStream = new FileOutputStream(path);
                byte[] data = new byte[1024];
                while (true) {
                    int len = inputStream.read(data);
                    if (-1 == len)
                        break;
                    fileOutputStream.write(data);
                }
                inputStream.close();
                fileOutputStream.close();
            } catch (IOException e) {
                e.printStackTrace();
                initOk = false;
            }
        }

        if (initOk) {
            initMagic(path);
            status = STATUS.INITED;

            if (null != listener) {
                listener.initFinish();
            }
        } else {
            status = STATUS.INIT_FAIL;
        }
    }

    public void init() {
        if (STATUS.UN_INIT == status || STATUS.INIT_FAIL == status) {
            thread.start();

            status = STATUS.INITING;
        }
    }

    public String getFileFormat(String path) {
        if (STATUS.INITED == status) {
            return null == path ? null : getFileMimeType(path);
        } else {
            return null;
        }
    }

    public void setListener(InitFinishListener listener) {
        this.listener = listener;
    }
}
