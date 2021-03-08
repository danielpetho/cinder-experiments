from face_sdk_3divi import Config, FacerecService, Error
from face_sdk_3divi.modules import video_worker
from face_sdk_3divi.modules.raw_image import RawImage, Format
from face_sdk_3divi.example import CVRawImage

from picamera.array import PiRGBArray
from picamera import PiCamera
import time
import cv2

def custom_tracking_callback(data: video_worker.TrackingCallbackData, userdata):
    
    assert data
    
    print("stream_id: " + str(data.stream_id))
    print("frame_id: " + str(data.frame_id))
    age_gender = data.samples_track_age_gender
    print("age_gender estimator: ")
    print(age_gender)
    

if __name__ == '__main__':
    try:
        face_sdk_root_dir = "/home/pi/face-sdk"
        service = FacerecService.create_service(face_sdk_root_dir + "/lib/libfacerec.so",     face_sdk_root_dir + "/conf/facerec")
        recognizer = service.create_recognizer("method7v7_recognizer.xml", True, True, False)
        capturer_config = Config("common_capturer4_fda.xml")
        capturer = service.create_capturer(capturer_config)
        print("facerec service initialized...")

        #video_worker = FacerecService.createVideoWorker()
        video_worker_config = Config("video_worker_lbf.xml")
        #video_worker_config.override_parameter("search_k", 3)

        video_worker_params = video_worker.Params()
        video_worker_params.video_worker_config = video_worker_config
        #video_worker_params.recognizer_ini_file = "method7v7_recognizer.xml", True, True, False
        video_worker_params.streams_count = 1
        video_worker_params.processing_threads_count = 0
        video_worker_params.matching_threads_count = 0
        video_worker_params.age_gender_estimation_threads_count = 1
        video_worker_params.emotions_estimation_threads_count = 0

        video_worker = service.create_video_worker(video_worker_params)
        
        print("video_worker initialized...")
        # ...
    except Error as e:
        print(e)

     # initialize the camera and grab a reference to the raw camera capture
    camera = PiCamera()
    
    stream_id = 0
    camera.resolution = (800, 600)
    camera.framerate = 32
    rawCapture = PiRGBArray(camera, size=(800, 600))
    # allow the camera to warmup
    time.sleep(0.1)
    i = 0
    # capture frames from the camera
    track_data = []
    tracking_callback_id = video_worker.add_tracking_callback_u(custom_tracking_callback, track_data)

    for frame in camera.capture_continuous(rawCapture, format="bgr", use_video_port=True):

        image = frame.array
        
        if(i % 5 == 0):
            raw_img = CVRawImage(image)
            x = video_worker.add_video_frame(raw_img, stream_id)
            
        
        i+=1
        cv2.imshow("Frame", image)
        key = cv2.waitKey(1) & 0xFF
        # clear the stream in preparation for the next frame
        rawCapture.truncate(0)
        # if the `q` key was pressed, break from the loop
        if key == ord("q"):
            break
