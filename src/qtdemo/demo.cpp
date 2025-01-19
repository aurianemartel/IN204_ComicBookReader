#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QPixmap>
#include <QMessageBox>
#include <QFileDialog>
#include <quazip5/quazip.h>
#include <quazip5/quazipfile.h>
#include <QtConcurrent/QtConcurrent>
#include <QFutureWatcher>
#include <QPainter>

#include <thread>
#include <chrono>

class SimpleComicViewer : public QWidget {
    Q_OBJECT

public:
    int sleepTime = 0;
    QString filePath = "";

    SimpleComicViewer(QWidget *parent = nullptr) : QWidget(parent), currentIndex(-1) {
        // Setup layout
        QVBoxLayout *mainLayout = new QVBoxLayout(this);
        QHBoxLayout *controlLayout = new QHBoxLayout();

        // Create widgets
        imageLabel = new QLabel("Image not loaded");
        imageLabel->setAlignment(Qt::AlignCenter);
        //imagePathEdit = new QLineEdit();
        QPushButton *browseButton = new QPushButton("Load");
        //QPushButton *loadButton = new QPushButton("Load Archive");
        QPushButton *prevButton = new QPushButton("Previous");
        QPushButton *nextButton = new QPushButton("Next");
        //QPushButton *exitButton = new QPushButton("Exit");

        // Setup layout
        mainLayout->addWidget(imageLabel);
        //controlLayout->addWidget(imagePathEdit);
        //controlLayout->addWidget(loadButton);
        controlLayout->addWidget(prevButton);
        controlLayout->addWidget(browseButton);
        controlLayout->addWidget(nextButton);
        //controlLayout->addWidget(exitButton);
        mainLayout->addLayout(controlLayout);

        // Connect signals and slots
        connect(browseButton, &QPushButton::clicked, this, &SimpleComicViewer::browseArchive);
        //connect(loadButton, &QPushButton::clicked, this, &SimpleComicViewer::loadArchive);
        connect(prevButton, &QPushButton::clicked, this, &SimpleComicViewer::showPreviousImage);
        connect(nextButton, &QPushButton::clicked, this, &SimpleComicViewer::showNextImage);
        //connect(exitButton, &QPushButton::clicked, this, &QWidget::close);

        // Initialize placeholder pixmap
        placeholderPixmap = QPixmap(800, 600);
        placeholderPixmap.fill(Qt::gray);
        QPainter painter(&placeholderPixmap);
        painter.drawText(placeholderPixmap.rect(), Qt::AlignCenter, "Please wait while loading...");
    }

private slots:
    void browseArchive() {
        filePath = QFileDialog::getOpenFileName(this, "Open Archive File", "", "Archives (*.zip);;Comics in CBZ format (*.cbz)");
        if (!filePath.isEmpty()) {
            //imagePathEdit->setText(filePath);
            loadArchive();
        }
    }

    void loadArchive() {
        //QString filePath = imagePathEdit->text();
        if (filePath.isEmpty()) {
            QMessageBox::warning(this, "Warning", "Please enter a valid file path.");
            return;
        }

        QuaZip zip(filePath);
        if (!zip.open(QuaZip::mdUnzip)) {
            QMessageBox::warning(this, "Warning", "Failed to open archive.");
            return;
        }

        zipFiles.clear();
        images.clear();

        for (bool more = zip.goToFirstFile(); more; more = zip.goToNextFile()) {
            QuaZipFile file(&zip);
            if (!file.open(QIODevice::ReadOnly)) {
                QMessageBox::warning(this, "Warning", "Failed to read file in archive.");
                continue;
            }

            QString fileName = file.getActualFileName();
            if (fileName.endsWith(".jpg", Qt::CaseInsensitive) || 
                fileName.endsWith(".jpeg", Qt::CaseInsensitive) || 
                fileName.endsWith(".bmp", Qt::CaseInsensitive) || 
                fileName.endsWith(".png", Qt::CaseInsensitive)) {
                zipFiles.append(fileName);
                images.append(QPixmap()); // Placeholder for the image
            }
            file.close();
        }

        zip.close();

        if (zipFiles.isEmpty()) {
            QMessageBox::warning(this, "Warning", "No images found in the archive.");
            return;
        }

        currentIndex = 0;
        loadImagesAsync();
        showCurrentImage();
    }

    void loadImagesAsync() {
        QFutureWatcher<void> *watcher = new QFutureWatcher<void>(this);
        connect(watcher, &QFutureWatcher<void>::finished, this, &SimpleComicViewer::onImagesLoaded);

        QFuture<void> future = QtConcurrent::run([this]() {
            QuaZip zip(filePath);
            if (!zip.open(QuaZip::mdUnzip)) return;

            for (int i = 0; i < zipFiles.size(); ++i) {
                zip.setCurrentFile(zipFiles[i]);
                QuaZipFile file(&zip);
                if (!file.open(QIODevice::ReadOnly)) continue;

                QByteArray imageData = file.readAll();
                QPixmap pixmap;
                pixmap.loadFromData(imageData);

                images[i] = pixmap;
                file.close();

                // Trigger UI update after loading each image
                QMetaObject::invokeMethod(this, "showCurrentImage", Qt::QueuedConnection);
                std::this_thread::sleep_for(std::chrono::seconds(sleepTime));
            }
            zip.close();
        });

        watcher->setFuture(future);
    }

    void onImagesLoaded() {
        showCurrentImage();
    }

    void showCurrentImage() {
        if (currentIndex < 0 || currentIndex >= images.size()) return;

        QPixmap pixmap = images[currentIndex];
        if (pixmap.isNull()) {
            imageLabel->setPixmap(placeholderPixmap.scaled(imageLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        } else {
            imageLabel->setPixmap(pixmap.scaled(imageLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
    }

    void showPreviousImage() {
        if (currentIndex > 0) {
            currentIndex--;
            showCurrentImage();
        }
    }

    void showNextImage() {
        if (currentIndex < images.size() - 1) {
            currentIndex++;
            showCurrentImage();
        }
    }

private:
    QLabel *imageLabel;
    //QLineEdit *imagePathEdit;
    QList<QPixmap> images;
    QStringList zipFiles;
    int currentIndex;
    QPixmap placeholderPixmap;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QCommandLineParser parser;
    parser.setApplicationDescription("CBR/CBZ viewer");
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption optionNumber(QStringList() << "n" << "sleep_time",
                                    "Sleep time for async loader",
                                    "value");
    parser.addOption(optionNumber);

    // Parse the command line
    parser.process(app);

    SimpleComicViewer viewer;

    if (parser.isSet(optionNumber)) {
        bool ok;
        int number = parser.value(optionNumber).toInt(&ok);
        if (ok) {
            viewer.sleepTime = number;
        }
    }
    
    viewer.setWindowTitle("Image Viewer");
    viewer.resize(800, 600);
    viewer.show();
    return app.exec();
}

#include "demo.moc"
