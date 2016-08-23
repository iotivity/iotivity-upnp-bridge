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
 * Link
 * <p/>
 * This class is used by UpnpAvClientActivity to create an object representation of a link of a remote device
 * and update the values depending on the server response
 */
public class Link {

    public static final String HREF_KEY = "href";
    public static final String REL_KEY = "rel";
    public static final String RT_KEY = "rt";

    private String mHref;
    private String mRel;
    private String mRt;

    public Link() {
        mHref = "";
        mRel = "";
        mRt = "";
    }

    public void setOcRepresentation(OcRepresentation ocRep) throws OcException {
        if (ocRep.hasAttribute(HREF_KEY)) {
            mHref = ocRep.getValue(HREF_KEY);
        }
        if (ocRep.hasAttribute(REL_KEY)) {
            mRel = ocRep.getValue(REL_KEY);
        }
        if (ocRep.hasAttribute(RT_KEY)) {
            mRt = ocRep.getValue(RT_KEY);
        }

    }

    public OcRepresentation getOcRepresentation() throws OcException {
        OcRepresentation ocRep = new OcRepresentation();
        ocRep.setValue(HREF_KEY, mHref);
        ocRep.setValue(REL_KEY, mHref);
        ocRep.setValue(RT_KEY, mHref);
        return ocRep;
    }

    public String getHref() {
        return mHref;
    }

    public void setHref(String href) {
        mHref = href;
    }

    public String getRel() {
        return mRel;
    }

    public void setRel(String rel) {
        mRel = rel;
    }

    public String getRt() {
        return mRt;
    }

    public void setRt(String rt) {
        mRt = rt;
    }

    @Override
    public String toString() {
        return "[" + HREF_KEY + ": " + getHref() +
                ", " + REL_KEY + ": " + getRel() +
                ", " + RT_KEY + ": " + getRt() + "]";
    }
}
