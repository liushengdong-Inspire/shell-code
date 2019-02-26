package vixtel.com.swfirstlogin;

import android.app.Activity;
import android.app.ProgressDialog;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.media.MediaPlayer;
import android.net.Uri;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.ListView;
import android.widget.ProgressBar;
import android.widget.TextView;

import org.json.JSONArray;
import org.json.JSONObject;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.URL;
import java.net.URLConnection;
import java.util.HashMap;
import java.util.Map;

public class MainActivity extends Activity implements View.OnClickListener,MediaPlayer.OnPreparedListener,MediaPlayer.OnCompletionListener{

    private String TAG = "LSD";
    private Button mPlayBtn;//播放
    private Button mPauseBtn;//暂停
    private Button mStopBtn;//停止
    private Button mPrvBtn;//上一首
    private Button mNextBtn;//下一首
    private TextView mMusiName;
    private ProgressBar mProgress;
    private ListView mMusicInfoListView;
    private ImageView mImageView = null;
    private boolean isStop = true; //停止
    private boolean isPause = false; //暂停
    private boolean isLoaded = false;//加载过 URL
    private MediaPlayer mMediaPlayer = null;
    private String mp3JSONPath = "http://10.10.15.53/mp3_json.xml";
    private String mLocalMp3JSONPath = null;
    private int jsonFileSize = -1;
    private String serverAddress = "http://10.10.15.53/";
    private String musicPath  = "";
    private ProgressDialog mProgressDialog = null;
    private int musicNumber = 0;
    private int musicCounter = 0;
    private boolean isInit  = false;
    private final static int ONE_SECOND = 1000;

    private final static int MSG_PROGRESS_COUNTER = 1;
    private final static int MSG_LOAD_JSON_FILE = 2;
    private final static int MSG_PARSE_JSON = 3;
    private final static int MSG_INIT_PARAM = 4;
    private final static int MSG_SET_MUSIC_NAME = 5;
    private final static int MSG_DOWNLOAD_PICTURE = 6;

    Mp3MapInfo mp3NameInfo;
    Mp3MapInfo mp3Info;
    Mp3MapInfo mp3PicInfo;

    private Handler mHandler = new Handler(new Handler.Callback() {
        @Override
        public boolean handleMessage(Message message) {
            switch (message.what) {
                case MSG_PROGRESS_COUNTER:
                    counterProgress();
                    break;
                case MSG_LOAD_JSON_FILE:
                    new Thread(new Runnable() {
                        @Override
                        public void run() {
                            loadMp3Json();
                        }
                    }).start();
                    break;
                case MSG_PARSE_JSON:
                    new Thread(new Runnable() {
                        @Override
                        public void run() {
                            parseJsonFile();
                        }
                    }).start();

                    break;
                case MSG_INIT_PARAM:
                    initData();
                    break;
                case MSG_SET_MUSIC_NAME:
                    Log.d(TAG," mp3Info size = "+mp3Info.getInstance().size());
                    if (musicNumber < mp3Info.getInstance().size()) {
                        String txt = mp3Info.getValue(musicNumber);
                        if (txt.length() > mp3NameInfo.getValue(musicNumber).length()) {
                            mMusiName.setText(txt);
                        }else {
                            String name[] = mp3NameInfo.getValue(musicNumber).split(".mp3");
                            mMusiName.setText(name[0]);
                        }

                        Log.d(TAG,"begin to setImage");
                        if(mp3PicInfo.getValue(musicNumber).equals("")) {
                            mImageView.setImageResource(R.mipmap.lsd);
                        }else {
                            String picturePath = mp3PicInfo.getValue(musicNumber);
                            String fileName = picturePath.substring(picturePath.lastIndexOf("/")+1);
                            String path = Environment.getExternalStorageDirectory().getAbsolutePath();
                            String pic = path+"/"+fileName;
                            Log.d(TAG,"picture path = "+pic);
                            try {
                                FileInputStream fis = new FileInputStream(pic);
                                int sizeRead = fis.read();
                                Log.d(TAG,"sizeRead = "+sizeRead);
                                mImageView.setImageURI(Uri.parse(pic));
                                //Bitmap bitmap = BitmapFactory.decodeFile("/mnt/sdcard/sz.jpg");
                                //mImageView.setImageBitmap(bitmap);
                            }catch (Exception e) {
                                Log.d(TAG, "error: " + e.getMessage(), e);
                            }
                        }
                    }
                    break;
                case MSG_DOWNLOAD_PICTURE:
                    new Thread(new Runnable() {
                        @Override
                        public void run() {
                            downLoadPicture();
                        }
                    }).start();
                    break;
            }
            return false;
        }
    });

    private void downLoadPicture()
    {
        Log.d(TAG,"downLoadPicture");
        String path = Environment.getExternalStorageDirectory().getAbsolutePath();
        for (int i = 0;i < mp3PicInfo.getInstance().size();i ++)
        {
            String picturePath = mp3PicInfo.getValue(i);
            if (!picturePath.equals("")) {
                Log.d(TAG,"picture path = "+picturePath);
                String fileName = picturePath.substring(picturePath.lastIndexOf("/")+1);
                Log.d(TAG,"picture name = "+fileName);
                try {
                    String serverPath = serverAddress+picturePath;
                    Log.d(TAG,"serverPath = "+serverPath);
                    URL downloadUrl = new URL(serverPath);
                    URLConnection conn = downloadUrl.openConnection();
                    conn.connect();
                    Log.d(TAG,"connected!");
                    InputStream is = conn.getInputStream();
                    if (is == null )throw new RuntimeException("picture download Error!");
                    File file = new File(path);
                    if (!file.exists())
                        file.mkdirs();
                    String pictureSavePath = path+"/"+fileName;
                    FileOutputStream fos = new FileOutputStream(pictureSavePath);
                    byte[] buff = new byte[1024];
                    do {
                        int readSize = is.read(buff);
                        if (readSize == -1)
                            break;
                        fos.write(buff,0,readSize);
                    }while (true);
                    Log.d(TAG,"picture download success!");
                    fos.close();
                    is.close();
                }catch (Exception e) {
                    Log.d(TAG, "error: " + e.getMessage(), e);
                }
            }
        }
        mProgressDialog.cancel();
        mHandler.sendEmptyMessage(MSG_INIT_PARAM);
    }

    private  void loadMp3Json()
    {
        Log.d(TAG,"Enter");
        String path = Environment.getExternalStorageDirectory().getAbsolutePath();
        final long startTime = System.currentTimeMillis();
        //下载函数
        String filename=mp3JSONPath.substring(mp3JSONPath.lastIndexOf("/") + 1);
        //获取文件名
        Log.d(TAG,"Enter filename = "+filename);
        try{
            URL myURL = new URL(mp3JSONPath);
            URLConnection conn = myURL.openConnection();
            conn.connect();
            Log.d(TAG,"Enter connected");
            InputStream is = conn.getInputStream();
            jsonFileSize = conn.getContentLength();
            if (jsonFileSize <= 0 )
                Log.d(TAG,"Enter jsonFileSize = "+jsonFileSize);
            Log.d(TAG,"size = "+conn.getContentLength()+" ");
            if (is == null) throw new RuntimeException("stream is null");
            File file1 = new File(path);
            if(!file1.exists()){
                file1.mkdirs();
            }
            //把数据存入路径+文件名
            mLocalMp3JSONPath = path+"/"+filename;
            FileOutputStream fos = new FileOutputStream(mLocalMp3JSONPath);
            byte buf[] = new byte[1024];
            int downLoadFileSize = 0;
            do{
                //循环读取
                int numread = is.read(buf);
                if (numread == -1)
                {
                    break;
                }
                fos.write(buf, 0, numread);
                downLoadFileSize += numread;
                int pross = (int)(((double)downLoadFileSize/(double)jsonFileSize)*100);
                mProgressDialog.setProgress(pross);
                //更新进度条
            } while (true);
            Log.i(TAG,"download success");
            fos.close();
            is.close();
            mProgressDialog.setProgress(100);
            mHandler.sendEmptyMessage(MSG_PARSE_JSON);
        } catch (Exception ex) {
            Log.d(TAG, "error: " + ex.getMessage(), ex);
            mHandler.sendEmptyMessageDelayed(MSG_LOAD_JSON_FILE,ONE_SECOND);
        }
    }

    private  void parseJsonFile()
    {
        if( mLocalMp3JSONPath == null )
        {
            Log.d(TAG,"mLocalMp3JSONPath = null");
            return;
        }

        FileInputStream fileInputStream = null;
        try {
            fileInputStream = new FileInputStream(mLocalMp3JSONPath);
            StringBuilder sb = new StringBuilder("");
            byte []buff = new byte[1024];
            int len = 0;
            len = fileInputStream.read(buff);
            while ((len > 0)) {
                sb.append(new String(buff,0,len));
                len = fileInputStream.read(buff);
            }
            String content = new String(sb);
            JSONArray jsonArray = new JSONArray(content);
            for (int i = 0;i < jsonArray.length();i ++)
            {
                JSONObject jsonObject = jsonArray.getJSONObject(i);
                String name = jsonObject.getString("name");
                String title = jsonObject.getString("title");
                String singer = jsonObject.getString("singer");
                String picturePath = jsonObject.getString("picpath");
                mp3NameInfo.setValue(i,name);
                mp3Info.setValue(i,title+"/"+singer);
                mp3PicInfo.setValue(i,picturePath);
            }
            fileInputStream.close();
            mHandler.sendEmptyMessage(MSG_DOWNLOAD_PICTURE);
        } catch (Exception e) {
            if (fileInputStream != null)
                try {
                    fileInputStream.close();
                } catch (IOException e1) {
                    e1.printStackTrace();
                }
            Log.d(TAG,"Error: "+e.getMessage(),e);
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        if( mProgressDialog == null ) {
            mProgressDialog = new ProgressDialog(this);
        }
        mp3NameInfo = new Mp3MapInfo();
        mp3PicInfo = new Mp3MapInfo();
        mp3Info = new Mp3MapInfo();
        mProgressDialog.setTitle("加载列表");
        mProgressDialog.setMessage("列表加载中，请稍后......");
        mProgressDialog.setProgressStyle(ProgressDialog.STYLE_HORIZONTAL);
        mProgressDialog.show();
        mHandler.sendEmptyMessage(MSG_LOAD_JSON_FILE);
    }

    private void counterProgress() {
        if (mMediaPlayer!=null && mMediaPlayer.isPlaying()) {
            double musicLength = mMediaPlayer.getDuration();
            double curretPos = mMediaPlayer.getCurrentPosition();
            int progressNum = (int)((curretPos/musicLength)*100);
            if (musicCounter != progressNum ) {
                mProgress.setProgress(progressNum);
                musicCounter = progressNum;
                Log.d(TAG,"progressNum =" +progressNum);
            }
            if (isStop) {
                mProgress.setProgress(0);
            }
        }
        mHandler.sendEmptyMessage(MSG_PROGRESS_COUNTER);
    }

    private void initData() {
        mPlayBtn = (Button)findViewById(R.id.playBtn);
        mPlayBtn.setOnClickListener(this);
        mPauseBtn = (Button)findViewById(R.id.pauseBtn);
        mPauseBtn.setOnClickListener(this);
        mStopBtn = (Button)findViewById(R.id.stopBtn);
        mStopBtn.setOnClickListener(this);
        mPrvBtn = (Button)findViewById(R.id.prevBtn);
        mPrvBtn.setOnClickListener(this);
        mNextBtn = (Button)findViewById(R.id.nextBtn);
        mNextBtn.setOnClickListener(this);
        mMusiName = (TextView)findViewById(R.id.musicName);
        mProgress = (ProgressBar)findViewById(R.id.musicProgress);
        mMusicInfoListView = (ListView)findViewById(R.id.musicInfoListView);
        mImageView = (ImageView)findViewById(R.id.musicImageView);
        final int mp3NameInfoSize = mp3NameInfo.getInstance().size();
        String [] stringsArray = new String[mp3NameInfoSize];
        for (int i = 0;i < mp3NameInfoSize;i ++)
            stringsArray[i] = mp3NameInfo.getValue(i);
        ArrayAdapter<String> arrayAdapter = new ArrayAdapter<String>(this,android.R.layout.simple_list_item_1,stringsArray);
        mMusicInfoListView.setAdapter(arrayAdapter);
        mMusicInfoListView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> adapterView, View view, int i, long l) {
                Log.d(TAG,"select "+mp3NameInfo.getValue(i));
                musicNumber = i;
                String playUrl = serverAddress+mp3NameInfo.getValue(i);
                musicPath = playUrl;
                Log.d(TAG,"musicPath = "+musicPath);
                stopPlayer();
                initPlayer(musicPath);
            }
        });
        //mHandler.sendEmptyMessage(MSG_PROGRESS_COUNTER);
    }

    private boolean initPlayer(String mp3Path) {

        //加载提示框
        if (mProgressDialog == null ) {
            mProgressDialog = new ProgressDialog(this);

        }
        mProgressDialog.setTitle("音乐");
        mProgressDialog.setMessage("资源加载中，请稍后......");
        mProgressDialog.setCancelable(false);

        //mediaPlayer 为空
        if (mMediaPlayer == null ) {
            mMediaPlayer = new MediaPlayer();
        }

        //停止了，需要重新加载
        if ( isStop ) {
            try {
                isStop = false;
                isLoaded = true;
                isPause = false;
                mMediaPlayer.setDataSource(mp3Path);
                mMediaPlayer.prepareAsync();
                mMediaPlayer.setOnPreparedListener(this);
                mMediaPlayer.setOnCompletionListener(this);
                mProgressDialog.show();
                return true;
            }catch (Exception e ) {
                e.printStackTrace();
            }
        }
        return false;
    }

    private void stopPlayer() {
        if (mMediaPlayer != null) {
            try {
                isStop = true;
                isLoaded = false;
                mMediaPlayer.stop();
                mMediaPlayer.reset();
                mMediaPlayer.release();
                mMediaPlayer = null;
            }catch (Exception e ) {
                e.printStackTrace();
            }
        }
    }

    @Override
    protected void onResume() {
        if (isInit) {
            if (initPlayer(musicPath)) {
                super.onResume();
                return;
            }

            if (!isPause) {
                mMediaPlayer.start();
            }
        }
        super.onResume();
    }

    @Override
    protected void onPause() {
        if (isInit) {
            if (mMediaPlayer != null && mMediaPlayer.isPlaying()) {
                mMediaPlayer.pause();
            }
        }
        super.onPause();
    }

    @Override
    public void onClick(View view) {
        switch (view.getId()) {
            case R.id.playBtn:
                if (initPlayer(musicPath)) {
                    return;
                }
                if (isPause) {
                    mMediaPlayer.start();
                    isPause = false;
                }
                break;
            case R.id.pauseBtn:
                //必须是未停止，在播放，media 非空情况下，才可以暂停
                if (mMediaPlayer != null &&! isStop && mMediaPlayer.isPlaying()) {
                    isPause = true;
                    mMediaPlayer.pause();
                }
                break;
            case R.id.stopBtn:
                stopPlayer();
                break;
            case R.id.prevBtn:
                musicNumber --;
                if (musicNumber < 0) {
                    musicNumber = mp3NameInfo.getInstance().size() - 1 ;
                }
                stopPlayer();
                String namePrev = serverAddress+mp3NameInfo.getValue(musicNumber);
                initPlayer(namePrev);
                break;
            case R.id.nextBtn:
                musicNumber ++;
                if (musicNumber >= mp3NameInfo.getInstance().size() ) {
                    musicNumber = 0;
                }
                stopPlayer();
                String nameNext = serverAddress+mp3NameInfo.getValue(musicNumber);
                initPlayer(nameNext);
                break;
        }
    }

    @Override
    public void onPrepared(MediaPlayer mediaPlayer) {
        Log.d(TAG,"onPrepared!");
        if (mediaPlayer != null ) {
            //不是暂停或者停止状态，直接开始播放音乐
            if (!isStop && !isPause) {
                mProgressDialog.dismiss();
                mediaPlayer.start();
                mHandler.sendEmptyMessage(MSG_SET_MUSIC_NAME);
                mHandler.sendEmptyMessageDelayed(MSG_PROGRESS_COUNTER,ONE_SECOND);
            }
        }
    }

    @Override
    public void onCompletion(MediaPlayer mediaPlayer) {
        Log.d(TAG,"onCompletion!");
        musicNumber ++;
        if (musicNumber >= mp3NameInfo.getInstance().size() ) {
            musicNumber = 0;
        }
        stopPlayer();
        initPlayer(serverAddress+mp3NameInfo.getValue(musicNumber));
    }
}
