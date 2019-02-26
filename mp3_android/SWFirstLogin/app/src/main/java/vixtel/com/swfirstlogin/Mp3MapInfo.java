package vixtel.com.swfirstlogin;

import java.util.HashMap;
import java.util.Map;

/**
 * Created by Inspire on 2019/2/26.
 */

class Mp3MapInfo {
    private Map<Integer,String> mMap;

    public Map<Integer,String> getInstance() {
      return mMap;
    };

    public Mp3MapInfo() {
        mMap = new HashMap<>();
    };

    protected void setValue(int index,String info) {
        if (this.mMap == null || this.mMap.size()==0)
            mMap = new HashMap<>();
        mMap.put(index,info);
    };

    protected String getValue(int index) {
        if (this.mMap == null || this.mMap.size() == 0)
            return "";
        else
            return mMap.get(index);
    };
}
