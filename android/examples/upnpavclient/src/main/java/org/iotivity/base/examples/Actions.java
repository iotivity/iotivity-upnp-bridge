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

import org.iotivity.base.OcException;
import org.iotivity.base.OcRepresentation;

/**
 * Actions
 *
 * This class is used by UpnpAvClientActivity to create an object representation of the media control actions
 * and update the values depending on the server response
 */
public class Actions {

    public static final String ACTIONS_KEY = "actions";

    private Action[] mActions;

    public Actions() {
        mActions = new Action[0];
    }

    public void setOcRepresentation(OcRepresentation[] ocRepActions) throws OcException {
        if ((ocRepActions != null) && (ocRepActions.length > 0)) {
            mActions = new Action[ocRepActions.length];
            int index = 0;
            for (OcRepresentation ocRepAction : ocRepActions) {
                mActions[index] = new Action();
                mActions[index].setOcRepresentation(ocRepAction);
                ++index;
            }
        }
    }

    public OcRepresentation[] getOcRepresentation() throws OcException {
        OcRepresentation[] ocRepActions = new OcRepresentation[mActions.length];
        int index = 0;
        for (Action action : mActions) {
            OcRepresentation ocRepAction = action.getOcRepresentation();
            ocRepActions[index] = ocRepAction;
            ++index;
        }
        return ocRepActions;
    }

    public Action[] getActions() {
        return mActions;
    }

    public void setActions(Action[] actions) {
        mActions = (actions != null) ? actions : new Action[0];
    }

    @Override
    public String toString() {
        StringBuilder sb = new StringBuilder();
        sb.append("[");
        boolean first = true;
        for (Action action : mActions) {
            if (!first) {
                sb.append(", ");
            }
            if (action != null) {
                sb.append(action.toString());
            }
            first = false;
        }
        sb.append("]");

        return sb.toString();
    }
}
