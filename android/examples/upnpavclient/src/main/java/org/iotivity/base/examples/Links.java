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
 * Links
 * <p/>
 * This class is used by UpnpAvClientActivity to create an object representation of the links of a remote device
 * and update the values depending on the server response
 */
public class Links {

    public static final String LINKS_KEY = "links";

    private Link[] mLinks;

    public Links() {
        mLinks = new Link[0];
    }

    public void setOcRepresentation(OcRepresentation[] ocRepLinks) throws OcException {
        if ((ocRepLinks != null) && (ocRepLinks.length > 0)) {
            mLinks = new Link[ocRepLinks.length];
            int index = 0;
            for (OcRepresentation ocRepLink : ocRepLinks) {
                mLinks[index] = new Link();
                mLinks[index].setOcRepresentation(ocRepLink);
                ++index;
            }
        }
    }

    public OcRepresentation[] getOcRepresentation() throws OcException {
        OcRepresentation[] ocRepLinks = new OcRepresentation[mLinks.length];
        int index = 0;
        for (Link link : mLinks) {
            OcRepresentation ocRepLink = link.getOcRepresentation();
            ocRepLinks[index] = ocRepLink;
            ++index;
        }
        return ocRepLinks;
    }

    public Link[] getLinks() {
        return mLinks;
    }

    public void setLinks(Link[] links) {
        mLinks = (links != null) ? links : new Link[0];
    }

    @Override
    public String toString() {
        StringBuilder sb = new StringBuilder();
        sb.append("[");
        boolean first = true;
        for (Link link : mLinks) {
            if (!first) {
                sb.append(", ");
            }
            if (link != null) {
                sb.append(link.toString());
            }
            first = false;
        }
        sb.append("]");

        return sb.toString();
    }
}
