from face_sdk_3divi import Config, FacerecService, Error
from picamera.array import PiRGBArray
from picamera import PiCamera
import time
import cv2

if __name__ == '__main__':
    try:
        face_sdk_root_dir = "/home/pi/face-sdk"
        service = FacerecService.create_service(face_sdk_root_dir + "/lib/libfacerec.so",     face_sdk_root_dir + "/conf/facerec")
        recognizer = service.create_recognizer("method7v7_recognizer.xml", True, True, False)
        capturer_config = Config("common_capturer4_fda.xml")
        capturer = service.create_capturer(capturer_config)
        print("initialized...")

        video_worker_config = Config("video_worker_lbf.xml")
        #video_worker_config.override_parameter("search_k", 3)

        video_worker_params = video_worker.Params()
        video_worker_params.video_worker_config = video_worker_config
        video_worker_params.recognizer_ini_file = recognizer_config
        video_worker_params.streams_count = 1
        video_worker_params.processing_threads_count = 0
        video_worker_params.matching_threads_count = 0
        video_worker_params.age_gender_estimation_threads_count = 1
        video_worker_params.emotions_estimation_threads_count = 0

        video_worker = service.create_video_worker(video_worker_params)
        # ...
    except Error as e:
        print(e)

     # initialize the camera and grab a reference to the raw camera capture
    camera = PiCamera()
    camera.resolution = (640, 480)
    camera.framerate = 32
    rawCapture = PiRGBArray(camera, size=(640, 480))
    # allow the camera to warmup
    time.sleep(0.1)
    # capture frames from the camera
    for frame in camera.capture_continuous(rawCapture, format="bgr", use_video_port=True):
        # grab the raw NumPy array representing the image, then initialize the timestamp
        # and occupied/unoccupied text
        image = frame.array
        # CVRawImage = CVRawImage(image)
        #capturer.capture(image)
        # show the frame
        cv2.imshow("Frame", image)
        key = cv2.waitKey(1) & 0xFF
        # clear the stream in preparation for the next frame
        rawCapture.truncate(0)
        # if the `q` key was pressed, break from the loop
        if key == ord("q"):
            break
