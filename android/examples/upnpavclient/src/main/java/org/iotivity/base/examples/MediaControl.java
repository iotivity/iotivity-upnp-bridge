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

import org.iotivity.base.OcException;
import org.iotivity.base.OcRepresentation;

/**
 * MediaControl
 *
 * This class is used by UpnpAvClientActivity to create an object representation of a remote media control resource
 * and update the values depending on the server response
 */
public class MediaControl extends Service {

    public static final String OIC_TYPE_MEDIA_CONTROL = "oic.r.media.control";
    public static final String OCF_OIC_URI_PREFIX_MEDIA_CONTROL = "/ocf/media-control/";
    public static final String UPNP_OIC_URI_PREFIX_MEDIA_CONTROL = "/upnp/media-control/";

    public static final String STATE_KEY = "playState";
    public static final boolean DEFAULT_STATE = false;

    public static final String SPEED_KEY = "mediaSpeed";
    public static final Number DEFAULT_SPEED = 1;

    public static final String LOCATION_KEY = "mediaLocation";
    public static final String DEFAULT_LOCATION = "0";

    private boolean mPlayState;
    private Number mMediaSpeed;
    private String mMediaLocation;
    private boolean mIsInitialized;

    // TODO: needs array of actions (use Link/Links as prototype)

    public MediaControl() {
        super();
        mPlayState = DEFAULT_STATE;
        mMediaSpeed = DEFAULT_SPEED;
        mMediaLocation = DEFAULT_LOCATION;
    }

    public void setOcRepresentation(OcRepresentation rep) throws OcException {
        super.setOcRepresentation(rep);

        boolean stateIsInitialized = false;
        boolean speedIsInitialized = false;
        boolean locationIsInitialized = false;

        if (rep.hasAttribute(STATE_KEY)) {
            Object obj = rep.getValue(STATE_KEY);
            if (obj instanceof Boolean) {
                mPlayState = (Boolean) obj;
                stateIsInitialized = true;
            }
        }
        if (rep.hasAttribute(SPEED_KEY)) {
            Object obj = rep.getValue(SPEED_KEY);
            if (obj instanceof Number) {
                mMediaSpeed = (Number) obj;
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
            if (obj instanceof Integer) {
                // frame relative to current location
                mMediaLocation = ((Integer) obj).toString();
                locationIsInitialized = true;
            }
            if (obj instanceof Number) {
                // rate control
                mMediaLocation = ((Number) obj).toString();
                locationIsInitialized = true;
            }
        }

        // one of any is required
        mIsInitialized = (stateIsInitialized || speedIsInitialized || locationIsInitialized);
    }

    public OcRepresentation getOcRepresentation() throws OcException {
        OcRepresentation rep = super.getOcRepresentation();
        rep.setValue(STATE_KEY, mPlayState);
        rep.setValue(SPEED_KEY, mMediaSpeed.doubleValue());
        rep.setValue(LOCATION_KEY, mMediaLocation);
        return rep;
    }

    public boolean getPlayState() {
        return mPlayState;
    }

    public void setPlayState(boolean playState) {
        mPlayState = playState;
    }

    public Number getMediaSpeed() {
        return mMediaSpeed;
    }

    public void setMediaSpeed(Number mediaSpeed) {
        mMediaSpeed = mediaSpeed;
    }

    public String getMediaLocation() {
        return mMediaLocation;
    }

    public void setMediaLocation(String mediaLocation) {
        mMediaLocation = mediaLocation;
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
                ", " + LOCATION_KEY + ": " + mMediaLocation + "]";
    }
}
