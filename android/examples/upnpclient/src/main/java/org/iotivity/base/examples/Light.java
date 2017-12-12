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
 * Light
 * <p/>
 * This class is used by UpnpClientActivity to create an object representation of a remote light device
 * and update the values depending on the server response
 */
public class Light extends Device {

    public static final String OIC_TYPE_DEVICE_LIGHT = "oic.d.light";
    public static final String OCF_OIC_URI_PREFIX_LIGHT = "/ocf/light/";
    public static final String UPNP_OIC_URI_PREFIX_LIGHT = "/upnp/light/";
    public static final String OIC_URI_PREFIX_LIGHT = "/light/";


    private BinarySwitch mBinarySwitch;
    private Brightness mBrightness;
    private Configuration mConfig;

    private String mName = "Unknown";
    private boolean mState = BinarySwitch.DEFAULT_VALUE;
    private int mLightLevel = Brightness.DEFAULT_BRIGHTNESS;

    public Light() {
        super();
    }

    public void setOcRepresentation(OcRepresentation rep) throws OcException {
        super.setOcRepresentation(rep);
        if (rep.hasAttribute(Configuration.NAME_KEY)) {
            Object obj = rep.getValue(Configuration.NAME_KEY);
            if (obj instanceof String) {
                mName = (String) obj;
            }
        }
        if (rep.hasAttribute(BinarySwitch.VALUE_KEY)) {
            Object obj = rep.getValue(BinarySwitch.VALUE_KEY);
            if (obj instanceof Boolean) {
                mState = (Boolean) obj;
            }
        }
        if (rep.hasAttribute(Brightness.BRIGHTNESS_KEY)) {
            Object obj = rep.getValue(Brightness.BRIGHTNESS_KEY);
            if (obj instanceof Integer) {
                mLightLevel = (Integer) obj;
            }
        }
    }

    public OcRepresentation getOcRepresentation() throws OcException {
        OcRepresentation rep = super.getOcRepresentation();
        rep.setValue(Configuration.NAME_KEY, mName);
        rep.setValue(BinarySwitch.VALUE_KEY, mState);
        rep.setValue(Brightness.BRIGHTNESS_KEY, mLightLevel);
        return rep;
    }

    public BinarySwitch getBinarySwitch() {
        return mBinarySwitch;
    }

    public void setBinarySwitch(BinarySwitch binarySwitch) {
        mBinarySwitch = binarySwitch;
    }

    public Brightness getBrightness() {
        return mBrightness;
    }

    public void setBrightness(Brightness brightness) {
        mBrightness = brightness;
    }

    public Configuration getConfiguration() {
        return mConfig;
    }

    public void setConfiguration(Configuration config) {
        mConfig = config;
    }

    public String getName() {
        if (mConfig != null) {
            mName = mConfig.getName();
        }

        return mName;
    }

    public void setName(String name) {
        if (mConfig != null) {
            mConfig.setName(name);

        } else {
            mName = name;
        }
    }

    public boolean getState() {
        if (mBinarySwitch != null) {
            mState = mBinarySwitch.getValue();
        }

        return mState;
    }

    public void setState(boolean state) {
        if (mBinarySwitch != null) {
            mBinarySwitch.setValue(state);

        } else {
            mState = state;
        }
    }

    public int getLightLevel() {
        if (mBrightness != null) {
            mLightLevel = mBrightness.getBrightness();
        }

        return mLightLevel;
    }

    public void setLightLevel(int lightLevel) {
        if (mBrightness != null) {
            mBrightness.setBrightness(lightLevel);

        } else {
            mLightLevel = lightLevel;
        }
    }

    @Override
    public String toString() {
        String binarySwitchAsString = mBinarySwitch != null ? mBinarySwitch.toString() : "binary switch is null";
        String brightnessAsString = mBrightness != null ? mBrightness.toString() : "brightness is null";
        String configAsString = mConfig != null ? mConfig.toString() : "configuration is null";

        if (mConfig != null) {
            return "[" + super.toString() +
                    ", " + binarySwitchAsString +
                    ", " + brightnessAsString +
                    ", " + configAsString + "]";
        } else {
            return "[" + super.toString() +
                    ", " + Configuration.NAME_KEY + ": " + mName +
                    ", " + BinarySwitch.VALUE_KEY + ": " + mState +
                    ", " + Brightness.BRIGHTNESS_KEY + ": " + mLightLevel + "]";
        }
    }
}
