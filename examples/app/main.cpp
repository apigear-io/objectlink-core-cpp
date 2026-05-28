// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2022 ApiGear UG (haftungsbeschränkt)
// Copyright (c) 2022-2026 Epic Games, Inc.

#include <QtGui>
#include <QtQml>
#include "../qtolink/olinkclient.h"
#include "calcsink.h"

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QGuiApplication app(argc, argv);

    OLinkClient client(nullptr, QClientRegistry::getInstance(), nullptr);
    client.connectToHost(QUrl("ws://127.0.0.1:8182"));
    client.linkObjectSource("demo.Calc");

    qmlRegisterType<CalcSink>("net.olink", 1, 0, "Calculator");

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(
        &engine, &QQmlApplicationEngine::objectCreated,
        &app, [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
