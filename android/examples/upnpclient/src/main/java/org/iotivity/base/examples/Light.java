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

    public static final String UPNP_OIC_TYPE_DEVICE_LIGHT = "oic.d.light";
    public static final String UPNP_OIC_URI_PREFIX_LIGHT = "/upnp/oic/light/";

    private BinarySwitch mBinarySwitch;
    private Brightness mBrightness;

    public Light() {
        super();
    }

    public void setOcRepresentation(OcRepresentation rep) throws OcException {
        super.setOcRepresentation(rep);
    }

    public OcRepresentation getOcRepresentation() throws OcException {
        OcRepresentation rep = super.getOcRepresentation();
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

    public boolean getState() {
        boolean state = BinarySwitch.DEFAULT_VALUE;

        if (mBinarySwitch != null) {
            state = mBinarySwitch.getValue();
        }

        return state;
    }

    public void setState(boolean state) {
        if (mBinarySwitch != null) {
            mBinarySwitch.setValue(state);
        }
    }

    public int getLightLevel() {
        int lightLevel = Brightness.DEFAULT_BRIGHTNESS;

        if (mBrightness != null) {
            lightLevel = mBrightness.getBrightness();
        }

        return lightLevel;
    }

    public void setLightLevel(int lightLevel) {
        if (mBrightness != null) {
            mBrightness.setBrightness(lightLevel);
        }
    }

    @Override
    public String toString() {
        String binarySwitchAsString = mBinarySwitch != null ? mBinarySwitch.toString() : "binary switch is null";
        String brightnessAsString = mBrightness != null ? mBrightness.toString() : "brightness is null";
        return "[" + super.toString() +
                ", " + binarySwitchAsString +
                ", " + brightnessAsString + "]";
    }
}
