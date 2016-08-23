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
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.CompoundButton;
import android.widget.ImageView;
import android.widget.SeekBar;
import android.widget.Switch;
import android.widget.TextView;
import android.widget.Toast;

import org.iotivity.base.ErrorCode;
import org.iotivity.base.OcException;
import org.iotivity.base.OcHeaderOption;
import org.iotivity.base.OcRepresentation;
import org.iotivity.base.OcResource;

import java.net.URL;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class UpnpAvClientMediaRenderControlActivity extends Activity implements
        OcResource.OnPostListener {

    static public final String EXTRA_AV_TRANSPORT_OBJECT = "extra.av.transport.object";
    static public final String EXTRA_RENDERING_CONTROL_OBJECT = "extra.rendering.control.object";

    static private final String TAG = UpnpAvClientMediaRenderControlActivity.class.getSimpleName();
    private AvTransport mAvTransport;
    private RenderingControl mRenderingControl;
    private MediaItem mMediaItem;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_upnp_av_client_media_render_control);

        Bundle extras = getIntent().getExtras();
        mAvTransport = extras.getParcelable(EXTRA_AV_TRANSPORT_OBJECT);
        mRenderingControl = extras.getParcelable(EXTRA_RENDERING_CONTROL_OBJECT);
        mMediaItem = extras.getParcelable(UpnpAvClientSelectRendererActivity.EXTRA_MEDIA_ITEM_OBJECT);

        ((TextView) findViewById(R.id.name_text)).setText(mMediaItem.getDisplayName());
        ((TextView) findViewById(R.id.artist_text)).setText(mMediaItem.getArtist());
        ((TextView) findViewById(R.id.album_text)).setText(mMediaItem.getAlbum());

        final ImageView iconView = (ImageView) findViewById(R.id.album_art_icon);
        if ((mMediaItem.getAlbumArtUri() != null) && (!mMediaItem.getAlbumArtUri().isEmpty())) {
            Thread getBitmapThread = new Thread(new Runnable() {
                public void run() {
                    try {
                        URL url = new URL(mMediaItem.getAlbumArtUri());
                        final Bitmap bmp = BitmapFactory.decodeStream(url.openConnection().getInputStream());
                        runOnUiThread(new Runnable() {
                            public void run() {
                                iconView.setImageBitmap(bmp);
                            }
                        });

                    } catch (Exception e) {
                        Log.w(TAG, "Failed to get album art for url " + mMediaItem.getAlbumArtUri() + ", " + e.toString());
                    }
                }
            });
            getBitmapThread.start();
        }

        final Button playPauseButton = (Button) findViewById(R.id.play_pause_button);
        playPauseButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                new Thread(new Runnable() {
                    public void run() {
                        if (playPauseButton.getText().toString().equalsIgnoreCase("play")) {
                            Log.d(TAG, "Play media");
                            doActionAvTransportRepresentation("play");
                            runOnUiThread(new Runnable() {
                                public void run() {
                                    playPauseButton.setText(R.string.pause);
                                }
                            });

                        } else {
                            Log.d(TAG, "Pause media");
                            doActionAvTransportRepresentation("pause");
                            runOnUiThread(new Runnable() {
                                public void run() {
                                    playPauseButton.setText(R.string.play);
                                }
                            });
                        }
                    }
                }).start();
            }
        });

        final Button stopButton = (Button) findViewById(R.id.stop_button);
        stopButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                new Thread(new Runnable() {
                    public void run() {
                        Log.d(TAG, "Stop media");
                        doActionAvTransportRepresentation("stop");
                        runOnUiThread(new Runnable() {
                            public void run() {
                                playPauseButton.setText(R.string.play);
                            }
                        });
                    }
                }).start();
            }
        });

        ((Switch) findViewById(R.id.sound_switch)).setChecked(!mRenderingControl.isMute());
        ((SeekBar) findViewById(R.id.volume_bar)).setProgress(mRenderingControl.getVolume());
        ((TextView) findViewById(R.id.volume_text)).setText(String.format(getString(R.string.volume_level), mRenderingControl.getVolume()));

        ((Switch) findViewById(R.id.sound_switch)).setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, final boolean isChecked) {
                final int volume = ((SeekBar) findViewById(R.id.volume_bar)).getProgress();
                new Thread(new Runnable() {
                    public void run() {
                        postRenderingControlRepresentation(!isChecked, volume);
                    }
                }).start();
            }
        });

        ((SeekBar) findViewById(R.id.volume_bar)).setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, final int progress, boolean fromUser) {
                ((TextView) findViewById(R.id.volume_text)).setText(String.format(getString(R.string.volume_level), progress));
                final boolean soundOn = ((Switch) findViewById(R.id.sound_switch)).isChecked();
                new Thread(new Runnable() {
                    public void run() {
                        postRenderingControlRepresentation(!soundOn, progress);
                    }
                }).start();
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {
            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
            }
        });

        new Thread(new Runnable() {
            public void run() {
                doActionAvTransportRepresentation("play");
                runOnUiThread(new Runnable() {
                    public void run() {
                        playPauseButton.setText(R.string.pause);
                    }
                });
            }
        }).start();
    }

    private void doActionAvTransportRepresentation(String action) {
        Log.i(TAG, "Posting av transport representation for " + action + " action...");

        if ((mAvTransport != null) && (mAvTransport.isInitialized())) {
            OcRepresentation avTransportRepresentation = null;
            try {
                avTransportRepresentation = mAvTransport.getOcRepresentation();

                OcRepresentation embeddedOcRep = new OcRepresentation();
                avTransportRepresentation.setValue(action, embeddedOcRep);

            } catch (OcException e) {
                Log.e(TAG, "Failed to get OcRepresentation from a av transport -- " + e.toString(), e);
            }

            if (avTransportRepresentation != null) {
                Map<String, String> queryParams = new HashMap<>();
                try {
                    OcResource avTransportResource = UpnpAvClientSelectRendererActivity.getOcResourceFromUri(mAvTransport.getUri());
                    if (avTransportResource != null) {
                        avTransportResource.post(avTransportRepresentation, queryParams, this);

                    } else {
                        Log.e(TAG, "No av transport for uri " + mAvTransport.getUri());
                    }

                } catch (OcException e) {
                    Log.e(TAG, "Error occurred while invoking \"post\" API -- " + e.toString(), e);
                }
            }

        } else {
            runOnUiThread(new Runnable() {
                public void run() {
                    if (mAvTransport == null) {
                        Toast.makeText(UpnpAvClientMediaRenderControlActivity.this, "No av transport for uri " + mAvTransport.getUri(), Toast.LENGTH_LONG).show();

                    } else {
                        Toast.makeText(UpnpAvClientMediaRenderControlActivity.this, "No av transport (initialized) for uri " + mAvTransport.getUri(), Toast.LENGTH_LONG).show();
                    }
                }
            });
        }
    }

    private void postRenderingControlRepresentation(boolean isMute, int newVolume) {
        Log.i(TAG, "Posting rendering control representation...");

        if ((mRenderingControl != null) && (mRenderingControl.isInitialized())) {
            mRenderingControl.setMute(isMute);
            mRenderingControl.setVolume(newVolume);
            OcRepresentation renderingControlRepresentation = null;
            try {
                renderingControlRepresentation = mRenderingControl.getOcRepresentation();

            } catch (OcException e) {
                Log.e(TAG, "Failed to get OcRepresentation from a rendering control -- " + e.toString(), e);
            }

            if (renderingControlRepresentation != null) {
                Map<String, String> queryParams = new HashMap<>();
                try {
                    OcResource renderingControlResource = UpnpAvClientSelectRendererActivity.getOcResourceFromUri(mRenderingControl.getUri());
                    if (renderingControlResource != null) {
                        renderingControlResource.post(renderingControlRepresentation, queryParams, this);

                    } else {
                        Log.e(TAG, "No rendering control for uri " + mAvTransport.getUri());
                    }

                } catch (OcException e) {
                    Log.e(TAG, "Error occurred while invoking \"post\" API -- " + e.toString(), e);
                }
            }

        } else {
            runOnUiThread(new Runnable() {
                public void run() {
                    if (mRenderingControl == null) {
                        Toast.makeText(UpnpAvClientMediaRenderControlActivity.this, "No rendering control for uri " + mRenderingControl.getUri(), Toast.LENGTH_LONG).show();

                    } else {
                        Toast.makeText(UpnpAvClientMediaRenderControlActivity.this, "No rendering control (initialized) for uri " + mRenderingControl.getUri(), Toast.LENGTH_LONG).show();
                    }
                }
            });
        }
    }

    @Override
    public synchronized void onPostCompleted(List<OcHeaderOption> list, OcRepresentation ocRepresentation) {
        Log.i(TAG, "POST request was successful");
        Log.i(TAG, "Resource URI (from getUri()): " + ocRepresentation.getUri());

        try {
            if (ocRepresentation.hasAttribute(Device.URI_KEY)) {
                final String ocRepUri = ocRepresentation.getValue(Device.URI_KEY);
                Log.i(TAG, "Resource URI: " + ocRepUri);

            } else {
                Log.w(TAG, "No Resource URI");
            }

        } catch (OcException e) {
            Log.e(TAG, "Failed to create resource representation -- " + e.toString(), e);
        }
    }

    @Override
    public synchronized void onPostFailed(Throwable throwable) {
        if (throwable instanceof OcException) {
            OcException ocEx = (OcException) throwable;
            Log.e(TAG, ocEx.toString());
            ErrorCode errCode = ocEx.getErrorCode();
            // do something based on errorCode
            Log.e(TAG, "Error code: " + errCode);
        }

        Log.e(TAG, "Failed to \"post\" a new representation");
    }
}
