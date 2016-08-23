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

import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.ImageView;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

import org.iotivity.base.ErrorCode;
import org.iotivity.base.OcException;
import org.iotivity.base.OcHeaderOption;
import org.iotivity.base.OcRepresentation;
import org.iotivity.base.OcResource;
import org.xmlpull.v1.XmlPullParser;
import org.xmlpull.v1.XmlPullParserException;
import org.xmlpull.v1.XmlPullParserFactory;

import java.io.IOException;
import java.io.StringReader;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.Comparator;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Stack;

public class UpnpAvClientBrowseActivity extends Activity implements OcResource.OnGetListener {

    static public final String EXTRA_CONTENT_DIRECTORY_OBJECT = "extra.content.directory.object";
    static public final String EXTRA_OBJECT_ID = "extra.object.id";

    static public final String UPNP_OIC_QUERY_PARAM_OBJECT_ID = "oid";
    static public final String UPNP_OIC_QUERY_PARAM_BROWSE_FLAG = "bf";
    static public final String UPNP_OIC_QUERY_PARAM_FILTER = "f";
    static public final String UPNP_OIC_QUERY_PARAM_START_INDEX = "si";
    static public final String UPNP_OIC_QUERY_PARAM_REQUESTED_COUNT = "rc";
    static public final String UPNP_OIC_QUERY_PARAM_SORT_CRITERIA = "soc";
    static public final String UPNP_OIC_SEARCH_QUERY_PARAM_CONTAINER_ID = "cid";
    static public final String UPNP_OIC_SEARCH_QUERY_PARAM_SEARCH_CRITERIA = "sec";
    static public final String UPNP_OIC_SEARCH_QUERY_PARAM_FILTER = "sf";
    static public final String UPNP_OIC_SEARCH_QUERY_PARAM_START_INDEX = "ssi";
    static public final String UPNP_OIC_SEARCH_QUERY_PARAM_REQUESTED_COUNT = "src";
    static public final String UPNP_OIC_SEARCH_QUERY_PARAM_SORT_CRITERIA = "ssc";

    static private final String TAG = UpnpAvClientBrowseActivity.class.getSimpleName();
    private final Stack<String> mObjectIdStack = new Stack<>();
    private final Comparator<MediaObject> mMediaObjectComparator = new MediaObjectComparator();
    private final Object mArrayAdapterSync = new Object();
    private ArrayAdapter<MediaObject> mArrayAdapter;
    private List<MediaObject> mMediaObjectList;
    private ContentDirectory mContentDirectory;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_upnp_av_client_browse);

        mMediaObjectList = new ArrayList<MediaObject>() {
            @Override
            public boolean addAll(Collection<? extends MediaObject> mediaObjects) {
                synchronized (mArrayAdapterSync) {
                    mArrayAdapter.setNotifyOnChange(false);
                    mMediaObjectList.clear();
                    super.addAll(mediaObjects);
                    Collections.sort(mMediaObjectList, mMediaObjectComparator);
                    mArrayAdapter.setNotifyOnChange(true);
                }

                runOnUiThread(new Runnable() {
                    public void run() {
                        mArrayAdapter.notifyDataSetChanged();
                    }
                });
                return true;
            }
        };

        mArrayAdapter = new ArrayAdapter<MediaObject>(this, android.R.layout.simple_list_item_1, mMediaObjectList) {
            @Override
            public View getView(int position, View convertView, ViewGroup parent) {
                View view = convertView;
                if (view == null) {
                    LayoutInflater inflater = LayoutInflater.from(parent.getContext());
                    view = inflater.inflate(android.R.layout.simple_list_item_1, parent, false);
                }

                TextView text1 = (TextView) view.findViewById(android.R.id.text1);

                final MediaObject mediaObject = mArrayAdapter.getItem(position);
                if (mediaObject != null) {
                    text1.setText(String.format("%s", mediaObject.getDisplayName()));

                } else {
                    Toast.makeText(UpnpAvClientBrowseActivity.this, "No media object for position " + position, Toast.LENGTH_LONG).show();
                }

                return view;
            }
        };

        ListView listView = (ListView) findViewById(R.id.browse_list_view);
        listView.setAdapter(mArrayAdapter);

        listView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                final MediaObject mediaObject = mArrayAdapter.getItem((int) id);
                if (mediaObject != null) {
                    if (mContentDirectory != null) {
                        final OcResource ocResource = UpnpAvClientActivity.getOcResourceFromUri(mContentDirectory.getUri());
                        if (ocResource != null) {
                            new Thread(new Runnable() {
                                public void run() {
                                    if (mediaObject instanceof MediaItem) {
                                        // get metadata for this item, onGetCompleted() will launch the next acivity
                                        getResourceRepresentation(ocResource, mediaObject.getId(), true);

                                    } else {
                                        Log.d(TAG, "Pushing " + mediaObject.getId());
                                        mObjectIdStack.push(mediaObject.getId());
                                        getResourceRepresentation(ocResource, mediaObject.getId(), false);
                                    }
                                }
                            }).start();

                        } else {
                            Toast.makeText(UpnpAvClientBrowseActivity.this, "No OcResource for uri " + mContentDirectory.getUri(), Toast.LENGTH_LONG).show();
                        }

                    } else {
                        Toast.makeText(UpnpAvClientBrowseActivity.this, "No Content Directory Service", Toast.LENGTH_LONG).show();
                    }

                } else {
                    Toast.makeText(UpnpAvClientBrowseActivity.this, "No media object for position " + position, Toast.LENGTH_LONG).show();
                }
            }
        });

        Bundle extras = getIntent().getExtras();
        mContentDirectory = extras.getParcelable(EXTRA_CONTENT_DIRECTORY_OBJECT);
        final String objectId = extras.getString(EXTRA_OBJECT_ID);

        if (mContentDirectory != null) {
            Log.d(TAG, "contentDirectory = " + mContentDirectory.toString());
            final OcResource ocResource = UpnpAvClientActivity.getOcResourceFromUri(mContentDirectory.getUri());
            if (ocResource != null) {
                new Thread(new Runnable() {
                    public void run() {
                        Log.d(TAG, "Pushing " + objectId);
                        mObjectIdStack.push(objectId);
                        getResourceRepresentation(ocResource, objectId, false);
                    }
                }).start();

            } else {
                Toast.makeText(UpnpAvClientBrowseActivity.this, "No OcResource for uri " + mContentDirectory.getUri(), Toast.LENGTH_LONG).show();

            }

        } else {
            Toast.makeText(UpnpAvClientBrowseActivity.this, "No Content Directory Service", Toast.LENGTH_LONG).show();
        }
    }

    @Override
    public void onBackPressed() {
        String backObjectId = null;

        if (!mObjectIdStack.isEmpty()) {
            backObjectId = mObjectIdStack.pop();
            Log.d(TAG, "Popping " + backObjectId);
            if (!mObjectIdStack.isEmpty()) {
                backObjectId = mObjectIdStack.peek();
                Log.d(TAG, "Peeking " + backObjectId);
                if ((backObjectId != null) && (!backObjectId.isEmpty()) && (!backObjectId.equalsIgnoreCase("-1"))) {
                    OcResource ocResource = UpnpAvClientActivity.getOcResourceFromUri(mContentDirectory.getUri());
                    if (ocResource != null) {
                        getResourceRepresentation(ocResource, backObjectId, false);
                        return;

                    } else {
                        Toast.makeText(UpnpAvClientBrowseActivity.this, "No OcResource for uri " + mContentDirectory.getUri(), Toast.LENGTH_LONG).show();
                    }
                }
            }
        }

        super.onBackPressed();
    }

    private void getResourceRepresentation(OcResource ocResource, String objectId, boolean browseMetadata) {
        Log.i(TAG, "Getting Browse Result Representation...");

        Map<String, String> queryParams = new HashMap<>();
        queryParams.put(UPNP_OIC_QUERY_PARAM_OBJECT_ID, objectId.replaceAll("%", "%25")); // escape %
        queryParams.put(UPNP_OIC_QUERY_PARAM_BROWSE_FLAG, browseMetadata ? "BrowseMetadata" : "BrowseDirectChildren");
        queryParams.put(UPNP_OIC_QUERY_PARAM_FILTER, browseMetadata ? "*" : "");
        queryParams.put(UPNP_OIC_QUERY_PARAM_START_INDEX, "0");
        queryParams.put(UPNP_OIC_QUERY_PARAM_REQUESTED_COUNT, "100");
        queryParams.put(UPNP_OIC_QUERY_PARAM_SORT_CRITERIA, "");

        try {
            ocResource.get(queryParams, this);

        } catch (OcException e) {
            Log.e(TAG, "Error occurred while invoking \"get\" API -- " + e.toString(), e);
            String erroredObjectId = mObjectIdStack.pop(); // remove failed push
            Log.d(TAG, "Popping errored object" + erroredObjectId);
            showErrorDialog(e.toString());
        }
    }

    @Override
    public synchronized void onGetCompleted(List<OcHeaderOption> list, OcRepresentation ocRepresentation) {
        Log.i(TAG, "GET request was successful");
        Log.i(TAG, "Resource URI (from getUri()): " + ocRepresentation.getUri());

        try {
            if (ocRepresentation.hasAttribute(Device.URI_KEY)) {
                final String ocRepUri = ocRepresentation.getValue(Device.URI_KEY);
                Log.i(TAG, "Resource URI: " + ocRepUri);

                mContentDirectory.setOcRepresentation(ocRepresentation);
                Log.d(TAG, "Resource attributes: " + mContentDirectory.toString());

                String metadataXml = mContentDirectory.getBrowseResult();
                List<MediaObject> mediaObjects = parseMetadata(metadataXml);
                if ((mediaObjects.size() == 1) && (mediaObjects.get(0) instanceof MediaItem)) {
                    // launch next activity
                    MediaItem mediaItem = (MediaItem)mediaObjects.get(0);
                    mediaItem.setMetadata(metadataXml);
                    Intent intent = new Intent(UpnpAvClientBrowseActivity.this, org.iotivity.base.examples.UpnpAvClientSelectRendererActivity.class);
                    intent.putExtra(UpnpAvClientSelectRendererActivity.EXTRA_MEDIA_ITEM_OBJECT, mediaItem);
                    startActivity(intent);

                } else {
                    mMediaObjectList.addAll(mediaObjects); // uses overridden addAll()
                    Log.d(TAG, "mediaObjects:");
                    for (MediaObject mediaObject : mediaObjects) {
                        Log.d(TAG, mediaObject.toString());
                    }
                }

            } else {
                Log.w(TAG, "No Resource URI");
            }

        } catch (OcException e) {
            Log.e(TAG, "Failed to read the attributes of resource -- " + e.toString(), e);
        }
    }

    @Override
    public synchronized void onGetFailed(Throwable throwable) {
        if (throwable instanceof OcException) {
            OcException ocEx = (OcException) throwable;
            Log.e(TAG, ocEx.toString());
            ErrorCode errCode = ocEx.getErrorCode();
            // do something based on errorCode
            Log.e(TAG, "Error code: " + errCode);
            showErrorDialog("Error code = " + errCode);
        }

        Log.e(TAG, "Failed to get representation of a content directory resource");
    }

    private void showErrorDialog(final String errorMessage) {
        runOnUiThread(new Runnable() {
            public void run() {
                AlertDialog.Builder builder = new AlertDialog.Builder(UpnpAvClientBrowseActivity.this);
                builder.setTitle(R.string.get_browse_result_error_dialog_title);
                builder.setMessage(errorMessage);
                builder.setCancelable(true);
                builder.setNeutralButton(android.R.string.ok, new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int id) {
                    }
                });

                AlertDialog alert = builder.create();
                alert.setOnKeyListener(new DialogInterface.OnKeyListener() {
                    @Override
                    public boolean onKey(DialogInterface dialog, int keyCode, KeyEvent event) {
                        if (keyCode == KeyEvent.KEYCODE_BACK) {
                            dialog.dismiss();
                            return true;
                        }
                        return false;
                    }
                });
                alert.show();
            }
        });
    }

    private List<MediaObject> parseMetadata(String metadataXml) {
        List<MediaObject> mediaObjects = new ArrayList<>();

        if (metadataXml != null) {
            Log.i(TAG, "metadataXml: " + metadataXml);
            try {
                XmlPullParserFactory factory = XmlPullParserFactory.newInstance();
                factory.setNamespaceAware(true);
                XmlPullParser parser = factory.newPullParser();
                parser.setInput(new StringReader(metadataXml));
                int eventType = parser.getEventType();

                while (eventType != XmlPullParser.END_DOCUMENT) {
                    if (eventType == XmlPullParser.START_TAG) {
                        Log.d(TAG, "Start tag " + parser.getPrefix() + ":" + parser.getName());
                        if ("container".equalsIgnoreCase(parser.getName())) {
                            String id = parser.getAttributeValue(null, "id");
                            Log.d(TAG, "container id = " + id);
                            String parentId = parser.getAttributeValue(null, "parentID");
                            Log.d(TAG, "container parentId = " + parentId);
                            String displayName = id; // default for now
                            // TODO: other attributes

                            eventType = parser.next();
                            boolean doneParsingContainer = false;
                            while (!doneParsingContainer) {
                                if (eventType == XmlPullParser.START_TAG) {
                                    Log.d(TAG, "Start tag " + parser.getPrefix() + ":" + parser.getName());
                                    if ("dc".equalsIgnoreCase(parser.getPrefix()) && "title".equalsIgnoreCase(parser.getName())) {
                                        displayName = parser.nextText();
                                        Log.d(TAG, "dc:title = " + displayName);
                                    }
                                }
                                if (eventType == XmlPullParser.END_TAG) {
                                    Log.d(TAG, "End tag " + parser.getPrefix() + ":" + parser.getName());
                                    doneParsingContainer = "container".equalsIgnoreCase(parser.getName());
                                }
                                if (!doneParsingContainer) {
                                    eventType = parser.next();
                                }
                            }
                            MediaContainer container = new MediaContainer(id, parentId, displayName);
                            mediaObjects.add(container);
                        }

                        if ("item".equalsIgnoreCase(parser.getName())) {
                            String id = parser.getAttributeValue(null, "id");
                            Log.d(TAG, "item id = " + id);
                            String parentId = parser.getAttributeValue(null, "parentID");
                            Log.d(TAG, "item parentId = " + parentId);
                            String displayName = id; // default for now
                            String artist = null;
                            String album = null;
                            String albumArtUri = null;
                            String upnpClass = null;
                            String resource = null;
                            String mp3Resource = null;
                            // TODO: other attributes

                            eventType = parser.next();
                            boolean doneParsingItem = false;
                            while (!doneParsingItem) {
                                if (eventType == XmlPullParser.START_TAG) {
                                    Log.d(TAG, "Start tag " + parser.getPrefix() + ":" + parser.getName());
                                    if ("dc".equalsIgnoreCase(parser.getPrefix()) && "title".equalsIgnoreCase(parser.getName())) {
                                        displayName = parser.nextText();
                                        Log.d(TAG, "dc:title = " + displayName);
                                    }
                                    if ("upnp".equalsIgnoreCase(parser.getPrefix())) {
                                        if ("artist".equalsIgnoreCase(parser.getName())) {
                                            artist = parser.nextText();
                                            Log.d(TAG, "upnp:artist = " + artist);
                                        }
                                        if ("album".equalsIgnoreCase(parser.getName())) {
                                            album = parser.nextText();
                                            Log.d(TAG, "upnp:album = " + album);
                                        }
                                        if ("albumArtURI".equalsIgnoreCase(parser.getName())) {
                                            albumArtUri = parser.nextText();
                                            Log.d(TAG, "upnp:albumArtURI = " + albumArtUri);
                                        }
                                        if ("class".equalsIgnoreCase(parser.getName())) {
                                            upnpClass = parser.nextText();
                                            Log.d(TAG, "upnp:class = " + upnpClass);
                                        }
                                    }
                                    if ("res".equalsIgnoreCase(parser.getName())) {
                                        resource = parser.nextText();
                                        Log.d(TAG, "resource = " + resource);
                                        if (resource.toLowerCase().endsWith("mp3")) {
                                            mp3Resource = resource;
                                        }
                                    }
                                }
                                if (eventType == XmlPullParser.END_TAG) {
                                    Log.d(TAG, "End tag " + parser.getPrefix() + ":" + parser.getName());
                                    doneParsingItem = "item".equalsIgnoreCase(parser.getName());
                                }
                                if (!doneParsingItem) {
                                    eventType = parser.next();
                                }
                            }
                            MediaItem item = new MediaItem(id, parentId, displayName);
                            item.setArtist(artist);
                            item.setAlbum(album);
                            item.setAlbumArtUri(albumArtUri);
                            if ((upnpClass != null) && (upnpClass.toLowerCase().contains("audio"))) {
                                item.setResource((mp3Resource != null) ? mp3Resource : resource);
                            } else {
                                item.setResource(resource);
                            }
                            item.setItemClass(upnpClass);
                            mediaObjects.add(item);
                        }
                    }
                    if (eventType == XmlPullParser.END_TAG) {
                        Log.d(TAG, "End tag " + parser.getPrefix() + ":" + parser.getName());
                    }
                    eventType = parser.next();
                }

            } catch (IOException e) {
                Log.w(TAG, "IOException in parseMetadata() " + e.getMessage());
            } catch (XmlPullParserException e) {
                Log.w(TAG, "XmlPullParserException in parseMetadata() " + e.getMessage());
            }
        }

        return mediaObjects;
    }

    class MediaObjectComparator implements Comparator<MediaObject> {
        @Override
        public int compare(MediaObject lhs, MediaObject rhs) {
            return ((lhs.getDisplayName()).compareToIgnoreCase((rhs.getDisplayName())));
        }
    }
}
