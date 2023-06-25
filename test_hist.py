import numpy as np
import cv2
import myHist
import pytest
import time
import matplotlib.pyplot as plt

filename = 'img/lake.jpg'

def test_histEq():
    myHist_output = openCV_output = cv2.imread(filename)

    myHist_output[:, :, 0] = myHist.histEq_thread(myHist_output[:, :, 0])
    openCV_output[:, :, 0] = cv2.equalizeHist(openCV_output[:, :, 0])

    assert np.array_equal(myHist_output, openCV_output)

def test_histEq_serial_performance():
    my_img = opencv_img = cv2.imread(filename)

    start_time = time.time()
    my_img[:, :, 0] = myHist.histEq_serial(my_img[:, :, 0])
    my_hist_time = time.time() - start_time

    start_time = time.time()
    opencv_img[:, :, 0] = cv2.equalizeHist(opencv_img[:, :, 0])
    opencv_hist_time = time.time() - start_time

    performance = my_hist_time / opencv_hist_time
    assert my_hist_time < opencv_hist_time

def test_histEq_thread_performance():
    my_img = opencv_img = cv2.imread(filename)

    start_time = time.time()
    my_img[:, :, 0] = myHist.histEq_thread(my_img[:, :, 0])
    my_hist_time = time.time() - start_time

    start_time = time.time()
    opencv_img[:, :, 0] = cv2.equalizeHist(opencv_img[:, :, 0])
    opencv_hist_time = time.time() - start_time

    performance = my_hist_time / opencv_hist_time
    assert my_hist_time < opencv_hist_time

if __name__ == '__main__':
    files = ['img/acne-face.jpg', 'img/berry.jpg', 'img/lake.jpg', 'img/gray.jpg', 'img/flower.jpg', 'img/dark-mountain.jpg', 'img/forest.jpg']

    for f in files:
        my_img_serial = cv2.imread(f)
        my_img_thread = my_img_serial.copy()
        opencv_img = my_img_serial.copy()
        # print(hex(id(my_img_serial)), hex(id(my_img_thread)), hex(id(opencv_img)))
        print(f'file: {f}, shape: {my_img_serial.shape}')

        ycbcr = cv2.cvtColor(my_img_serial, cv2.COLOR_BGR2YCrCb)
        # plot the distrubution of Y channel
        plt.figure()
        plt.title(f'{f.split("/")[1].split(".")[0]}')
        plt.xlabel('Intensity Value')
        plt.ylabel('Count')
        plt.hist(ycbcr[:, :, 0].ravel(), 256, [0, 256], color='b', alpha=0.5)
        plt.savefig(f'output/{f.split("/")[1].split(".")[0]}-intensity.png')

        start_time = time.time()
        my_img_serial[:, :, 0] = myHist.histEq_serial(my_img_serial[:, :, 0])
        my_hist_serial_time = time.time() - start_time

        start_time = time.time()
        my_img_thread[:, :, 0] = myHist.histEq_thread(my_img_thread[:, :, 0])
        my_hist_thread_time = time.time() - start_time

        start_time = time.time()
        opencv_img[:, :, 0] = cv2.equalizeHist(opencv_img[:, :, 0])
        opencv_hist_time = time.time() - start_time
    
        if f == 'img/forest.jpg':
            plt.figure()
            plt.title(f'RGB')
            plt.xlabel('Intensity Value')
            plt.ylabel('Count') 
            rgb = cv2.imread(f)
            plt.hist(rgb[:,:,0].ravel(), 256, [0, 256], color='orange', alpha=0.5, label='B')
            plt.hist(rgb[:,:,1].ravel(), 256, [0, 256], color='b', alpha=0.5, label='G')
            plt.hist(rgb[:,:,2].ravel(), 256, [0, 256], color='g', alpha=0.5, label='R')
            plt.legend()
            plt.savefig(f'output/{f.split("/")[1].split(".")[0]}-rgb.png')

            plt.figure()
            plt.title(f'YCbCr')
            plt.xlabel('Intensity Value')
            plt.ylabel('Count') 
            ycbcr = cv2.cvtColor(rgb, cv2.COLOR_BGR2YCrCb)
            plt.hist(ycbcr[:,:,0].ravel(), 256, [0, 256], color='orange', alpha=0.5, label='Y')
            plt.hist(ycbcr[:,:,1].ravel(), 256, [0, 256], color='b', alpha=0.5, label='Cr')
            plt.hist(ycbcr[:,:,2].ravel(), 256, [0, 256], color='g', alpha=0.5, label='Cb')
            plt.legend()
            plt.savefig(f'output/{f.split("/")[1].split(".")[0]}-ycbcr.png')

            # ycbcr[:, :, 0] = cv2.equalizeHist(ycbcr[:, :, 0])

            # plt.hist(ycbcr[:,:,0].ravel(), 256, [0, 256], color='b', alpha=0.5)
            # plt.legend(('before', 'after'))
            # plt.savefig('output/color-after-histEq.png')

            # result = cv2.cvtColor(ycbcr, cv2.COLOR_YCrCb2BGR)
            # cv2.imwrite('output/color_hist.jpg', result)

        print(f'my_serial_histogram_time: {my_hist_serial_time*1000:.3f} ms')
        print(f'my_thread_histogram_time: {my_hist_thread_time*1000:.3f} ms')
        print(f'opencv_histogram_time: {opencv_hist_time*1000:.3f} ms')
        print('=====================================================')
    
    exec_time = [[0.074, 0.435, 3.243, 1.148], [0.053, 0.391, 2.748, 1.055], [0.042, 0.262, 5.527, 2.526]]
    plt.figure()
    plt.title('Performance Comparison')
    plt.xlabel('Execution Time (ms)')
    plt.ylabel('Image')
    height = 0.3
    x = np.arange(len(exec_time[0]))
    plt.barh(x, exec_time[0], height, color='orange', label='serial')
    plt.barh(x + height, exec_time[1], height, color='green', label='openmp')
    plt.barh(x + height*2, exec_time[2], height, color='royalblue', label='opencv')
    plt.yticks(x + height, ('gray', 'flower', 'acne-face', 'forest'))
    plt.legend()
    plt.savefig('output/performance.png')
