// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2022 ApiGear UG (haftungsbeschränkt)
// Copyright (c) 2022-2026 Epic Games, Inc.

#include <QtCore>
#include "../qtolink/olinkhost.h"
#include "olink/remotenode.h"
#include "calcsource.h"
#include "olink/remoteregistry.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    ApiGear::ObjectLink::RemoteRegistry registry;
    OLinkHost server(registry, nullptr);
    CalcSource source(registry);
    server.listen("localhost", 8182);
    return app.exec();
}
