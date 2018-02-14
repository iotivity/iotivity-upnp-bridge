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

import android.graphics.Bitmap;

import org.iotivity.base.OcException;
import org.iotivity.base.OcRepresentation;

/**
 * Device
 * <p/>
 * This class is used by UpnpAvClientActivity to create an object representation of a remote device
 * and update the values depending on the server response
 */
abstract public class Device extends Resource {

    public static final String MANUFACTURER_KEY = "manufacturer";
    public static final String MANUFACTURER_URL_KEY = "manufacturer_url";
    public static final String MODEL_DESC_KEY = "model_description";
    public static final String MODEL_NAME_KEY = "model_name";
    public static final String MODEL_NUMBER_KEY = "model_number";
    public static final String MODEL_URL_KEY = "model_url";
    public static final String SERIAL_NUMBER_KEY = "serial_number";
    public static final String PRESENTATION_URL_KEY = "presentation_url";
    public static final String ICON_URL_KEY = "icon_url";
    public static final String UPC_KEY = "upc";

    public static final String LINKS_KEY = "links";

    private String mManufacturer;
    private String mManufacturerUrl;
    private String mModelDescription;
    private String mModelName;
    private String mModelNumber;
    private String mModelUrl;
    private String mSerialNumber;
    private String mPresentationUrl;
    private String mIconUrl;
    private String mUpc;

    private Links mLinks;
    private boolean mHasLinksProperty;

    private Bitmap mIconBitmap;

    public Device() {
        super();

        mManufacturer = "";
        mManufacturerUrl = "";
        mModelDescription = "";
        mModelName = "";
        mModelNumber = "";
        mModelUrl = "";
        mSerialNumber = "";
        mPresentationUrl = "";
        mIconUrl = "";
        mUpc = "";
        mLinks = new Links();
    }

    public void setOcRepresentation(OcRepresentation rep) throws OcException {
        super.setOcRepresentation(rep);

        if (rep.hasAttribute(MANUFACTURER_KEY)) {
            mManufacturer = rep.getValue(MANUFACTURER_KEY);
        }
        if (rep.hasAttribute(MANUFACTURER_URL_KEY)) {
            mManufacturerUrl = rep.getValue(MANUFACTURER_URL_KEY);
        }
        if (rep.hasAttribute(MODEL_DESC_KEY)) {
            mModelDescription = rep.getValue(MODEL_DESC_KEY);
        }
        if (rep.hasAttribute(MODEL_NAME_KEY)) {
            mModelName = rep.getValue(MODEL_NAME_KEY);
        }
        if (rep.hasAttribute(MODEL_NUMBER_KEY)) {
            mModelNumber = rep.getValue(MODEL_NUMBER_KEY);
        }
        if (rep.hasAttribute(MODEL_URL_KEY)) {
            mModelUrl = rep.getValue(MODEL_URL_KEY);
        }
        if (rep.hasAttribute(SERIAL_NUMBER_KEY)) {
            mSerialNumber = rep.getValue(SERIAL_NUMBER_KEY);
        }
        if (rep.hasAttribute(PRESENTATION_URL_KEY)) {
            mPresentationUrl = rep.getValue(PRESENTATION_URL_KEY);
        }
        if (rep.hasAttribute(ICON_URL_KEY)) {
            mIconUrl = rep.getValue(ICON_URL_KEY);
        }
        if (rep.hasAttribute(UPC_KEY)) {
            mUpc = rep.getValue(UPC_KEY);
        }
        if (rep.hasAttribute(LINKS_KEY)) {
            OcRepresentation[] links = rep.getValue(LINKS_KEY);
            mLinks.setOcRepresentation(links);
            mHasLinksProperty = true;
        }
    }

    public OcRepresentation getOcRepresentation() throws OcException {
        OcRepresentation rep = super.getOcRepresentation();

        rep.setValue(MANUFACTURER_KEY, mManufacturer);
        rep.setValue(MANUFACTURER_URL_KEY, mManufacturerUrl);
        rep.setValue(MODEL_DESC_KEY, mModelDescription);
        rep.setValue(MODEL_NAME_KEY, mModelName);
        rep.setValue(MODEL_NUMBER_KEY, mModelNumber);
        rep.setValue(MODEL_URL_KEY, mModelUrl);
        rep.setValue(SERIAL_NUMBER_KEY, mSerialNumber);
        rep.setValue(PRESENTATION_URL_KEY, mPresentationUrl);
        rep.setValue(ICON_URL_KEY, mIconUrl);
        rep.setValue(UPC_KEY, mUpc);

        OcRepresentation[] links = mLinks.getOcRepresentation();
        if ((links != null) && (links.length > 0)) {
            rep.setValue(LINKS_KEY, links);
        }

        return rep;
    }

    public String getManufacturer() {
        return mManufacturer;
    }

    public void setManufacturer(String manufacturer) {
        mManufacturer = manufacturer;
    }

    public String getManufacturerUrl() {
        return mManufacturerUrl;
    }

    public void setManufacturerUrl(String manufacturerUrl) {
        mManufacturerUrl = manufacturerUrl;
    }

    public String getModelDescription() {
        return mModelDescription;
    }

    public void setModelDescription(String modelDescription) {
        mModelDescription = modelDescription;
    }

    public String getModelName() {
        return mModelName;
    }

    public void setModelName(String modelName) {
        mModelName = modelName;
    }

    public String getModelNumber() {
        return mModelNumber;
    }

    public void setModelNumber(String modelNumber) {
        mModelNumber = modelNumber;
    }

    public String getModelUrl() {
        return mModelUrl;
    }

    public void setModelUrl(String modelUrl) {
        mModelUrl = modelUrl;
    }

    public String getSerialNumber() {
        return mSerialNumber;
    }

    public void setSerialNumber(String serialNumber) {
        mSerialNumber = serialNumber;
    }

    public String getPresentationUrl() {
        return mPresentationUrl;
    }

    public void setPresentationUrl(String presentationUrl) {
        mPresentationUrl = presentationUrl;
    }

    public String getIconUrl() {
        return mIconUrl;
    }

    public void setIconUrl(String iconUrl) {
        mIconUrl = iconUrl;
    }

    public String getUpc() {
        return mUpc;
    }

    public void setUpc(String upc) {
        mUpc = upc;
    }

    public boolean hasLinksProperty() {
        return mHasLinksProperty;
    }

    public Links getLinks() {
        return mLinks;
    }

    public void setLinks(Links links) {
        mLinks = (links != null) ? links : new Links();
    }

    public Bitmap getIconBitmap() {
        return mIconBitmap;
    }

    public void setIconBitmap(Bitmap iconBitmap) {
        mIconBitmap = iconBitmap;
    }

    @Override
    public String toString() {
        return "[" + super.toString() +
                ", " + MANUFACTURER_KEY + ": " + mManufacturer +
                ", " + MANUFACTURER_URL_KEY + ": " + mManufacturerUrl +
                ", " + MODEL_NAME_KEY + ": " + mModelName +
                ", " + MODEL_NUMBER_KEY + ": " + mModelNumber +
                ", " + MODEL_URL_KEY + ": " + mModelUrl +
                ", " + MODEL_DESC_KEY + ": " + mModelDescription +
                ", " + SERIAL_NUMBER_KEY + ": " + mSerialNumber +
                ", " + PRESENTATION_URL_KEY + ": " + mPresentationUrl +
                ", " + ICON_URL_KEY + ": " + mIconUrl +
                ", " + UPC_KEY + ": " + mUpc +
                ", " + "hasLinksProp: " + mHasLinksProperty +
                ", " + LINKS_KEY + mLinks + "]";
    }
}
