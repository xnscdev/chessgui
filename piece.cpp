#include "piece.h"

#include <utility>

Piece::Piece(QString name) : name(std::move(name)) {
}
