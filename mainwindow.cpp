#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    std::string projectBasePath = QDir::homePath().toStdString() + "/yolov8_CPP_Inference_OpenCV_ONNX";

    bool runOnGPU = true;

    //
    // Pass in either:
    //
    // "/source/models/yolov8s.onnx"
    // or
    // "/source/models/yolov5s.onnx"
    //
    // To run Inference with yolov8/yolov5 (ONNX)
    //

    Inference inf(projectBasePath + "/source/models/yolov8s.onnx", cv::Size(640, 480),
                  projectBasePath + "/source/classes/classes.txt", runOnGPU);

    QFileDialog dialog(this);
    dialog.setWindowTitle("Select your images for inferencing");
    dialog.setDirectory(QString::fromStdString(projectBasePath) + "/source/data");
    dialog.setFileMode(QFileDialog::ExistingFiles);
    dialog.setNameFilter("images (*.jpg *.jpeg *.png *.bmp)");

    QStringList fileNames;
    if (dialog.exec())
        fileNames = dialog.selectedFiles();

    for (int i = 0; i < fileNames.size(); ++i)
    {
        cv::Mat frame = cv::imread(fileNames[i].toStdString());

        // Inference starts here...
        std::vector<Detection> output = inf.runInference(frame);

        int detections = output.size();
        std::cout << "Number of detections:" << detections << std::endl;

        for (int i = 0; i < detections; ++i)
        {
            Detection detection = output[i];

            cv::Rect box = detection.box;
            cv::Scalar color = detection.color;

            cv::rectangle(frame, box, color, 2);
            cv::putText(frame, (detection.className + ' ' + std::to_string(detection.confidence).substr(0, 4)), cv::Point(box.x, box.y - 5), cv::FONT_HERSHEY_SIMPLEX, 1, color, 3);
        }
        // Inference ends here...

        // This is only for preview purposes
        float scale = 0.6;
        cv::resize(frame, frame, cv::Size(frame.cols*scale, frame.rows*scale));
        cv::imshow("Inference", frame);

        cv::waitKey(-1);
    }
}

MainWindow::~MainWindow()
{
    cv::destroyAllWindows();
    delete ui;
}

