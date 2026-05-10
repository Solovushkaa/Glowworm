// else if (headers.value("Accept") == "application/octet-stream") {
//     qDebug() << "Ответ на range запрос";
//     if (headers.contains("Range")) {
//         qDebug() << "Range: " << headers.value("Range");

//         auto [start, end] = parseRange(
//             QString(headers.value("Range").toByteArray()));

//         if (start == -1 || end == -1) { // -1 == max(qint64)
//             qDebug() << "-----------------------------------";
//             return QHttpServerResponse(
//                 QHttpServerResponse::StatusCode::BadRequest);
//         }

//         qDebug() << "-----------------------------------";

//         QFile file("/" + path.s);
//         if (!file.open(QIODevice::ReadOnly)) {
//             qWarning() << "Ошибка доступа к файлу: " << file.fileName();
//         }

//         if (start > end) {
//             qWarning()
//             << "Некорректные границы файла: start(" << start << "), end("
//             << end << "), file size(" << file.size() << ")";
//         }

//         if (!file.seek(start)) {
//             qWarning() << "Ошибка чтения с позиции: " << start;
//         }

//         end = std::min(end, file.size() - 1);

//         // QFile *file = new QFile("large.bin");
//         // file->open(QIODevice::ReadOnly); // QFile наследует QIODevice → можно передавать напрямую
//         // auto reply = manager->post(request, file); // Qt будет читать файл по частям внутренним буфером (~64 КБ)
//         QByteArray data = file.read(end - start
//                                     + 1); // Не оптимизированный мусор

//         QHttpServerResponse response(data);
//         QHttpHeaders responseHeader;

//         responseHeader.append(QHttpHeaders::WellKnownHeader::ContentRange,
//                               "bytes " + QString::number(start) + "-"
//                                   + QString::number(end) + "/"
//                                   + QString::number(file.size()));

//         responseHeader.append(QHttpHeaders::WellKnownHeader::Server,
//                               m_hostKey);

//         response.setHeaders(responseHeader);

//         // С помощью потоковой передачи, без подгрузки в RAM можно передавать очень большие файлы
//         // При необходимости докачать диапазон будет просто выбираться иная логика
//         // httpServer.route("/download/<arg>", QHttpServerRequest::Method::Get,
//         //                  [](const QString &filename) -> QHttpServerResponse {
//         //                      QFile *file = new QFile("/path/" + filename);
//         //                      if (!file->open(QIODevice::ReadOnly)) {
//         //                          delete file;
//         //                          return QHttpServerResponse(QHttpServerResponder::StatusCode::NotFound);
//         //                      }
//         //                      return QHttpServerResponse(file); // ← передаём QIODevice
//         //                  });
//         return response; // Тоже нужно подумать
//     }
// }
