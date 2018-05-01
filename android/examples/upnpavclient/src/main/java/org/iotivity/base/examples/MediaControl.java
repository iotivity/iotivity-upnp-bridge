/*
 * //******************************************************************
 * //
 * // Copyright 2018 Intel Corporation All Rights Reserved.
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
 * MediaControl
 *
 * This class is used by UpnpAvClientActivity to create an object representation of a remote media control resource
 * and update the values depending on the server response
 */
public class MediaControl extends Service implements Parcelable {

    public static final String OIC_TYPE_MEDIA_CONTROL = "oic.r.media.control";
    public static final String OCF_OIC_URI_PREFIX_MEDIA_CONTROL = "/ocf/mediaControl/";
    public static final String UPNP_OIC_URI_PREFIX_MEDIA_CONTROL = "/upnp/mediaControl/";
    public static final String UPNP_OIC_URI_PREFIX_MEDIA_CONTROL_NM_HREF = "/upnp/mediaControl/AVTransport/";

    public static final String STATE_KEY = "playState";
    public static final boolean DEFAULT_STATE = false;

    public static final String SPEED_KEY = "mediaSpeed";
    public static final double DEFAULT_SPEED = 1.0;

    public static final String LOCATION_KEY = "mediaLocation";
    public static final String DEFAULT_LOCATION = "00:00:00";

    public static final String LAST_ACTION_KEY = "lastAction";
    public static final String DEFAULT_LAST_ACTION = "stop";

    public static final String ACTIONS_KEY = "actions";

    private boolean mPlayState;
    private Double mMediaSpeed;
    private String mMediaLocation;
    private String mLastAction;
    private Actions mActions;
    private boolean mIsInitialized;

    public MediaControl() {
        super();
        mPlayState = DEFAULT_STATE;
        mMediaSpeed = DEFAULT_SPEED;
        mMediaLocation = DEFAULT_LOCATION;
        mLastAction = DEFAULT_LAST_ACTION;
        mActions = new Actions();
    }

    public void setOcRepresentation(OcRepresentation rep) throws OcException {
        super.setOcRepresentation(rep);

        boolean stateIsInitialized = false;
        boolean speedIsInitialized = false;
        boolean locationIsInitialized = false;
        boolean lastActionIsInitialized = false;

        if (rep.hasAttribute(STATE_KEY)) {
            Object obj = rep.getValue(STATE_KEY);
            if (obj instanceof Boolean) {
                mPlayState = (Boolean) obj;
                stateIsInitialized = true;
            }
        }
        if (rep.hasAttribute(SPEED_KEY)) {
            Object obj = rep.getValue(SPEED_KEY);
            if (obj instanceof Double) {
                mMediaSpeed = (Double) obj;
                speedIsInitialized = true;
            }
        }
        if (rep.hasAttribute(LOCATION_KEY)) {
            Object obj = rep.getValue(LOCATION_KEY);
            if (obj instanceof String) {
                // uri or data-time in ISO8601 format
                mMediaLocation = (String) obj;
                locationIsInitialized = true;
            }
            if (obj instanceof Double) {
                mMediaLocation = ((Double) obj).toString();
                locationIsInitialized = true;
            }
        }
        if (rep.hasAttribute(LAST_ACTION_KEY)) {
            Object obj = rep.getValue(LAST_ACTION_KEY);
            if (obj instanceof String) {
                mLastAction = (String) obj;
                lastActionIsInitialized = true;
            }
        }
        if (rep.hasAttribute(ACTIONS_KEY)) {
            OcRepresentation[] actions = rep.getValue(ACTIONS_KEY);
            mActions.setOcRepresentation(actions);
        }

        // one of any is required
        mIsInitialized = (stateIsInitialized || speedIsInitialized || locationIsInitialized);
    }

    public OcRepresentation getOcRepresentation() throws OcException {
        OcRepresentation rep = super.getOcRepresentation();
        rep.setValue(STATE_KEY, mPlayState);
        rep.setValue(SPEED_KEY, mMediaSpeed);
        rep.setValue(LOCATION_KEY, mMediaLocation);
        rep.setValue(LAST_ACTION_KEY, mLastAction);

        if (mActions != null) {
            OcRepresentation[] actions = mActions.getOcRepresentation();
            if ((actions != null) && (actions.length > 0)) {
                rep.setValue(ACTIONS_KEY, actions);
            }
        }

        return rep;
    }

    public boolean getPlayState() {
        return mPlayState;
    }

    public void setPlayState(boolean playState) {
        mPlayState = playState;
    }

    public Double getMediaSpeed() {
        return mMediaSpeed;
    }

    public void setMediaSpeed(Double mediaSpeed) {
        mMediaSpeed = mediaSpeed;
    }

    public String getMediaLocation() {
        return mMediaLocation;
    }

    public void setMediaLocation(String mediaLocation) {
        mMediaLocation = mediaLocation;
    }

    public String getLastAction() {
        return mLastAction;
    }

    public void setLastAction(String lastAction) {
        mLastAction = lastAction;
    }

    public Actions getActions() {
        return mActions;
    }

    public void setActions(Actions actions) {
        mActions = (actions != null) ? actions : new Actions();
    }

    public boolean isInitialized() {
        return mIsInitialized;
    }

    @Override
    public String toString() {
        return "[" + super.toString() +
                ", " + "initialized: " + mIsInitialized +
                ", " + STATE_KEY + ": " + mPlayState +
                ", " + SPEED_KEY + ": " + mMediaSpeed +
                ", " + LOCATION_KEY + ": " + mMediaLocation +
                ", " + LAST_ACTION_KEY + ": " + mLastAction +
                ", " + ACTIONS_KEY + mActions + "]";
    }

    // Parcelable implementation
    public MediaControl(Parcel in) {
        readFromParcel(in);
    }

    public static final Parcelable.Creator CREATOR = new Parcelable.Creator() {
        public MediaControl createFromParcel(Parcel in) {
            return new MediaControl(in);
        }

        public MediaControl[] newArray(int size) {
            return new MediaControl[size];
        }
    };

    public void writeToParcel(Parcel dest, int flags) {
        dest.writeString(getName());
        dest.writeString(getUri());
        dest.writeByte((byte) (getPlayState() ? 1 : 0));
        dest.writeDouble(getMediaSpeed());
        dest.writeString(getMediaLocation());
        dest.writeString(getLastAction());
        dest.writeByte((byte) (isInitialized() ? 1 : 0));
    }

    private void readFromParcel(Parcel in) {
        setName(in.readString());
        setUri(in.readString());
        setPlayState(in.readByte() != 0);
        setMediaSpeed(in.readDouble());
        setMediaLocation(in.readString());
        setLastAction(in.readString());
        mIsInitialized = in.readByte() != 0;
    }

    public int describeContents() {
        return 0;
    }
}
