/*
 * Filename: /home/octo-ws/GstreamCustom/zedstream.cpp
 * Path: /home/octo-ws/GstreamCustom
 * Created Date: Tuesday, July 19th 2022, 3:05:12 pm
 * Author: octo-ws
 * 
 * Copyright (c) 2022 Your Company
 */

#include <iostream>
#include <gst/gst.h>


static GMainLoop *loop;

struct ZedData {
    GstElement *pipeline;
    GstElement *source;
    GstElement *fil1;
    GstElement *timeolay;

    GstElement *avc;
    GstElement *enc1;
    GstElement *enc2;
    GstElement *enc3;  // rtph264pay
    GstElement *rtspsink;

};


static gboolean my_bus_callback (GstBus * bus, GstMessage * message, gpointer data)
{
  // g_print ("Got %s message\n", GST_MESSAGE_TYPE_NAME (message));

  switch (GST_MESSAGE_TYPE (message)) {
    case GST_MESSAGE_ERROR:{
      GError *err;
      gchar *debug;

      gst_message_parse_error (message, &err, &debug);
      g_print ("Error: %s\n", err->message);
      g_error_free (err);
      g_free (debug);

      g_main_loop_quit (loop);
      break;
    }
    case GST_MESSAGE_EOS:
      /* end-of-stream */
      g_main_loop_quit (loop);
      break;
    default:
      /* unhandled message */
      break;
  }

    return TRUE; /* we want to be notified each time message is received on the bus */
}



int main(int argc, char **argv){

    ZedData data {} ;
    GstBus *bus;
    GstMessage *msg;
    guint bus_watch_id;
    GstStateChangeReturn ret;
    GstCaps *caps;

    std::string ip;

    std::string cam = "Zed";

    ip = argv[1] + cam ;

    std::cout<<ip<<std::endl;

    GError *error = nullptr;

    gst_init_check(&argc,&argv,&error);

    data.source = gst_element_factory_make("zedsrc","src");

    data.fil1 = gst_element_factory_make("capsfilter", "caps1");

    data.timeolay = gst_element_factory_make("clockoverlay","overlay");

    data.avc  = gst_element_factory_make("autovideconvert","avc");

    data.enc1 = gst_element_factory_make("x264enc","encoder");

    data.enc2 = gst_element_factory_make("h264parse","parser");

    data.enc3 = gst_element_factory_make("rtph264pay","pay");

    data.rtspsink = gst_element_factory_make("rtspclientsink","sink");

    data.pipeline = gst_pipeline_new("pipelineOne");

    if ( ! data.pipeline || ! data.source || !data.fil1 || !data.timeolay || !data.enc1 || !data.enc2 || !data.enc3 || ! data.avc || ! data.rtspsink) 
    {
        g_printerr("Not All Elements can be created");
    
    }

    /* Configure Gstreamer Element Properties */

    g_object_set(data.source,"camera-resolution",2,NULL);

    caps = gst_caps_from_string("video/x-raw,width=(int)1280,height=(int)720,format=(string)BGRA,framerate=(fraction)30/1");

    g_object_set(data.fil1,"caps",caps,NULL);

     g_object_set(data.timeolay, "time-format", "%D %H:%M:%S", NULL);

    gst_caps_unref(caps);

    g_object_set(data.enc1,"byte-stream",true,"tune","zerolatency","speed-preset",1,NULL);

    g_object_set(data.rtspsink,"location","ip",NULL);

    gst_bin_add_many(GST_BIN(data.pipeline),data.source,data.fil1,data.timeolay,data.avc,data.enc1,data.enc2,data.enc3,data.rtspsink,NULL);

    if(! gst_element_link_many(data.source,data.fil1,data.timeolay,data.avc,data.enc1,data.enc2,data.enc3,data.rtspsink,NULL))
    {
             g_printerr("Elements could not be linked");
             gst_object_unref(data.pipeline);
             return -1;
    }


    ret = gst_element_set_state(data.pipeline, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE) {
        g_printerr("unable to set the pipeline to playing state");
        gst_object_unref(data.pipeline);
        return -1;
    }



    bus = gst_pipeline_get_bus(GST_PIPELINE(data.pipeline));

    gst_bus_add_signal_watch(bus);

    g_signal_connect(G_OBJECT(bus),"message",G_CALLBACK(my_bus_callback),NULL);


    gst_object_unref(bus);

    loop = g_main_loop_new (NULL,FALSE);

    g_main_loop_run(loop);

     /* clean up */
    gst_element_set_state (data.pipeline, GST_STATE_NULL);
    gst_object_unref (data.pipeline);
    g_source_remove (bus_watch_id);
    g_main_loop_unref (loop);
   

    return 0;
 


}

