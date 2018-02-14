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
 * Action
 *
 * This class is used by UpnpAvClientActivity to create an object representation of a media control action
 * and update the values depending on the server response
 */
public class Action {

    public static final String HREF_KEY = "href";
    public static final String REL_KEY = "rel";
    public static final String ACTION_KEY = "action";
    public static final String ALLOWED_VALUES_KEY = "allowed-values";
    public static final String ALLOWED_RANGE_KEY = "allowed-range";
    public static final String RANGE_STEP_KEY = "range-step";

    private String mHref;
    private String[] mRel = {"self"};
    private String mAction = "stop";
    private Double[] mAllowedValues;
    private Double[] mAllowedRange;
    private Double mRangeStep;

    public Action() {
    }

    public void setOcRepresentation(OcRepresentation ocRep) throws OcException {
        if (ocRep.hasAttribute(HREF_KEY)) {
            mHref = ocRep.getValue(HREF_KEY);
        }
        if (ocRep.hasAttribute(REL_KEY)) {
            mRel = ocRep.getValue(REL_KEY);
        }
        if (ocRep.hasAttribute(ACTION_KEY)) {
            mAction = ocRep.getValue(ACTION_KEY);
        }
        if (ocRep.hasAttribute(ALLOWED_VALUES_KEY)) {
            Object[] objects = ocRep.getValue(ALLOWED_VALUES_KEY);
            int i = 0;
            for (Object obj : objects) {
                if (obj instanceof Integer) {
                    mAllowedValues[i] = ((Integer) obj).doubleValue();
                }
                if (obj instanceof Double) {
                    mAllowedValues[i] = ((Double) obj);
                }
                ++i;
            }
        }
        if (ocRep.hasAttribute(ALLOWED_RANGE_KEY)) {
            Object[] objects = ocRep.getValue(ALLOWED_RANGE_KEY);
            int i = 0;
            for (Object obj : objects) {
                if (obj instanceof Integer) {
                    mAllowedRange[i] = ((Integer) obj).doubleValue();
                }
                if (obj instanceof Double) {
                    mAllowedRange[i] = ((Double) obj);
                }
                ++i;
            }
        }
        if (ocRep.hasAttribute(RANGE_STEP_KEY)) {
            Object obj = ocRep.getValue(RANGE_STEP_KEY);
            if (obj instanceof Integer) {
                mRangeStep = ((Integer) obj).doubleValue();
            }
            if (obj instanceof Double) {
                mRangeStep = ((Double) obj);
            }
        }
    }

    public OcRepresentation getOcRepresentation() throws OcException {
        OcRepresentation ocRep = new OcRepresentation();
        if (mHref != null) {
            ocRep.setValue(HREF_KEY, mHref);
        }
        ocRep.setValue(REL_KEY, mRel);
        ocRep.setValue(ACTION_KEY, mAction);
        if (mAllowedValues != null) {
            double[] valuesAsDouble = new double[mAllowedValues.length];
            for (int i = 0; i < mAllowedValues.length; ++i) {
                valuesAsDouble[i] = mAllowedValues[i];
            }
            ocRep.setValue(ALLOWED_VALUES_KEY, valuesAsDouble);
        }
        if (mAllowedRange != null) {
            double[] rangeAsDouble = new double[mAllowedRange.length];
            for (int i = 0; i < mAllowedRange.length; ++i) {
                rangeAsDouble[i] = mAllowedRange[i];
            }
            ocRep.setValue(ALLOWED_RANGE_KEY, rangeAsDouble);
        }
        if (mRangeStep != null) {
            ocRep.setValue(RANGE_STEP_KEY, mRangeStep);
        }
        return ocRep;
    }

    public String getHref() {
        return mHref;
    }

    public void setHref(String href) {
        mHref = href;
    }

    public String[] getRel() {
        return mRel;
    }

    public void setRel(String rel[]) {
        if ((rel != null) && (rel.length > 0)) {
            mRel = rel;
        }
    }

    public String getAction() {
        return mAction;
    }

    public void setAction(String action) {
        if (action != null) {
            mAction = action;
        }
    }

    public Double[] getAllowedValues() {
        return mAllowedValues;
    }

    public void setAllowedValues(Double[] allowedValues) {
        if ((allowedValues != null) && (allowedValues.length > 0)) {
            mAllowedValues = allowedValues;
        }
    }

    public Double[] getAllowedRange() {
        return mAllowedRange;
    }

    public void setAllowedRange(Double[] allowedRange) {
        if ((allowedRange != null) && (allowedRange.length == 2)) {
            mAllowedRange = allowedRange;
        }
    }

    public Double getRangeStep() {
        return mRangeStep;
    }

    public void setRangeStep(Double rangeStep) {
        mRangeStep = rangeStep;
    }

    @Override
    public String toString() {
        StringBuilder sb = new StringBuilder();
        sb.append("[");
        boolean first = true;
        for (String type : getRel()) {
            if (!first) {
                sb.append(", ");
            }
            if (type != null) {
                sb.append(type.toString());
            }
            first = false;
        }
        sb.append("]");
        String relAsString = sb.toString();

        String allowedValuesAsString = null;
        if (getAllowedValues() != null) {
            sb = new StringBuilder();
            sb.append("[");
            first = true;
            for (Double value : getAllowedValues()) {
                if (!first) {
                    sb.append(", ");
                }
                if (value != null) {
                    sb.append(value.toString());
                }
                first = false;
            }
            sb.append("]");
            allowedValuesAsString = sb.toString();
        }

        String allowedRangeAsString = null;
        if (getAllowedRange() != null) {
            sb = new StringBuilder();
            sb.append("[");
            first = true;
            for (Double range : getAllowedRange()) {
                if (!first) {
                    sb.append(", ");
                }
                if (range != null) {
                    sb.append(range.toString());
                }
                first = false;
            }
            sb.append("]");
            allowedRangeAsString = sb.toString();
        }

        return "[" + HREF_KEY + ": " + getHref() +
                ", " + REL_KEY + ": " + relAsString +
                ", " + ACTION_KEY + ": " + getAction() +
                ", " + ALLOWED_VALUES_KEY + ": " + allowedValuesAsString +
                ", " + ALLOWED_RANGE_KEY + ": " + allowedRangeAsString +
                ", " + RANGE_STEP_KEY + ": " + getRangeStep() + "]";
    }
}
