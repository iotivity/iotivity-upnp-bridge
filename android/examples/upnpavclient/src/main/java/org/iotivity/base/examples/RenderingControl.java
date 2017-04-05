/*
 * //******************************************************************
 * //
 * // Copyright 2016 Intel Corporation All Rights Reserved.
 * //
 * //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 * //
 * // Licensed under the Apache License, Version 2.0 (the "License");
 * // you may not use this file except in compliance with the License.
 * // You may obtain a copy of the License at
 * //
 * //      http://www.apache.org/licenses/LICENSE-2.0
 * //
 * // Unless required by applicable law or agreed to in writing, software
 * // distributed under the License is distributed on an "AS IS" BASIS,
 * // WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * // See the License for the specific language governing permissions and
 * // limitations under the License.
 * //
 * //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 */

package org.iotivity.base.examples;

import android.os.Parcel;
import android.os.Parcelable;

import org.iotivity.base.OcException;
import org.iotivity.base.OcRepresentation;

/**
 * RenderingControl
 * <p/>
 * This class is used by UpnpAvClientActivity to create an object representation of a remote renderingControl service
 * and update the values depending on the server response
 */
public class RenderingControl extends Service implements Parcelable {

    public static final String UPNP_OIC_TYPE_RENDERING_CONTROL = "oic.r.rendering.control";
    public static final String UPNP_OIC_URI_PREFIX_RENDERING_CONTROL = "/upnp/rendering-control/";

    public static final String VOLUME_KEY = "volume";
    public static final String MUTE_KEY = "mute";
    public static final String DESIRED_VOLUME_KEY = "desiredVolume";
    public static final String DESIRED_MUTE_KEY = "desiredMute";
    public static final String LAST_CHANGE_KEY = "lastChange";
    public static final int DEFAULT_VOLUME = 0;
    public static final boolean DEFAULT_MUTE = false;

    private int mVolume;
    private boolean mMute;
    private String mLastChange;
    private boolean mIsInitialized;

    public RenderingControl() {
        super();
        mVolume = DEFAULT_VOLUME;
        mMute = DEFAULT_MUTE;
    }

    public void setOcRepresentation(OcRepresentation rep) throws OcException {
        super.setOcRepresentation(rep);
        if (rep.hasAttribute(LAST_CHANGE_KEY)) {
            Object obj = rep.getValue(LAST_CHANGE_KEY);
            if (obj instanceof String) {
                mLastChange = (String) obj;
                mIsInitialized = true;
            }
        }
        if (rep.hasAttribute(VOLUME_KEY)) {
            Object obj = rep.getValue(VOLUME_KEY);
            if (obj instanceof Integer) {
                mVolume = (Integer) obj;
            }
            if (obj instanceof OcRepresentation) {
                OcRepresentation embeddedOcRep = (OcRepresentation) obj;
                mVolume = embeddedOcRep.getValue(VOLUME_KEY);
            }
        }
        if (rep.hasAttribute(MUTE_KEY)) {
            Object obj = rep.getValue(MUTE_KEY);
            if (obj instanceof Boolean) {
                mMute = (Boolean) obj;
            }
            if (obj instanceof OcRepresentation) {
                OcRepresentation embeddedOcRep = (OcRepresentation) obj;
                mMute = embeddedOcRep.getValue(MUTE_KEY);
            }
        }
    }

    public OcRepresentation getOcRepresentation() throws OcException {
        OcRepresentation rep = super.getOcRepresentation();
//        rep.setValue(VOLUME_KEY, mVolume);
//        rep.setValue(MUTE_KEY, mMute);
        // Using embedded rep for now
        OcRepresentation embeddedOcRep = super.getOcRepresentation();
        embeddedOcRep.setValue(VOLUME_KEY, mVolume);
        embeddedOcRep.setValue(DESIRED_VOLUME_KEY, mVolume);
        embeddedOcRep.setValue(MUTE_KEY, mMute);
        embeddedOcRep.setValue(DESIRED_MUTE_KEY, mMute);
        rep.setValue(VOLUME_KEY, embeddedOcRep);
        rep.setValue(MUTE_KEY, embeddedOcRep);
        return rep;
    }

    public int getVolume() {
        return mVolume;
    }

    public void setVolume(int volume) {
        mVolume = Math.max(0, volume);
    }

    public boolean isMute() {
        return mMute;
    }

    public void setMute(boolean mute) {
        mMute = mute;
    }

    public String getLastChange() {
        return mLastChange;
    }

    public boolean isInitialized() {
        return mIsInitialized;
    }

    @Override
    public String toString() {
        return "[" + super.toString() +
                ", " + "initialized: " + mIsInitialized +
                ", " + VOLUME_KEY + ": " + mVolume +
                ", " + MUTE_KEY + ": " + mMute +
                ", " + LAST_CHANGE_KEY + ": " + mLastChange + "]";
    }

    // Parcelable implementation
    public RenderingControl(Parcel in) {
        readFromParcel(in);
    }

    public static final Parcelable.Creator CREATOR = new Parcelable.Creator() {
        public RenderingControl createFromParcel(Parcel in) {
            return new RenderingControl(in);
        }

        public RenderingControl[] newArray(int size) {
            return new RenderingControl[size];
        }
    };

    public void writeToParcel(Parcel dest, int flags) {
        dest.writeString(getName());
        dest.writeString(getUri());
        dest.writeInt(getVolume());
        dest.writeByte((byte) (isMute() ? 1 : 0));
        dest.writeByte((byte) (isInitialized() ? 1 : 0));
    }

    private void readFromParcel(Parcel in) {
        setName(in.readString());
        setUri(in.readString());
        setVolume(in.readInt());
        setMute(in.readByte() != 0);
        mIsInitialized = in.readByte() != 0;
    }

    public int describeContents() {
        return 0;
    }
}
