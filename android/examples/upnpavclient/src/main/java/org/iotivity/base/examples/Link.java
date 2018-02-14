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
    private Object mRt; // could be String or String[]

    public Link() {
        mHref = "";
        mRel = "";
        mRt = new Object();
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
        ocRep.setValue(REL_KEY, mRel);
        if (mRt instanceof String) {
            ocRep.setValue(RT_KEY, (String) mRt);
        } else if (mRt instanceof String[]) {
            ocRep.setValue(RT_KEY, (String[]) mRt);
        } else {
            // this is gonna fail...
        }
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

    public Object getRt() {
        return mRt;
    }

    public void setRt(Object rt) {
        mRt = rt;
    }

    @Override
    public String toString() {
        String rtAsString = null;
        Object rt = getRt();
        if (rt instanceof String) {
            rtAsString = (String) rt;

        } else if (rt instanceof String[]) {
            StringBuilder sb = new StringBuilder();
            sb.append("[");
            boolean first = true;
            for (String type : (String[]) rt) {
                if (!first) {
                    sb.append(", ");
                }
                if (type != null) {
                    sb.append(type.toString());
                }
                first = false;
            }
            sb.append("]");
            rtAsString = sb.toString();

        } else {
            rtAsString = "Unknown rt type of " + rt.getClass().getName();
        }

        return "[" + HREF_KEY + ": " + getHref() +
                ", " + REL_KEY + ": " + getRel() +
                ", " + RT_KEY + ": " + rtAsString + "]";
    }
}
