#include "app/LibraryCatalog.hpp"

namespace no::app {

const char* GetBookKey(BookId id) {
    switch (id) {
        case BookId::Marschbuch:
            return "marschbuch";
        case BookId::Kirchenbuch:
            return "kirchenbuch";
        case BookId::MareinerBlech:
            return "mareiner_blech";
        case BookId::Quartett:
            return "quartett";
        case BookId::BrassEnsemble:
            return "brassensemble";
        case BookId::Duette:
            return "duette";
        case BookId::Lieder:
            return "lieder";
        case BookId::Andere:
            return "andere";
        default:
            return "marschbuch";
    }
}

const char* GetBookTitle(BookId id) {
    switch (id) {
        case BookId::Marschbuch:
            return "Marschbuch";
        case BookId::Kirchenbuch:
            return "Kirchenbuch";
        case BookId::MareinerBlech:
            return "Mareiner Blech";
        case BookId::Quartett:
            return "Quartett";
        case BookId::BrassEnsemble:
            return "Brassensemble";
        case BookId::Duette:
            return "Duette";
        case BookId::Lieder:
            return "Lieder";
        case BookId::Andere:
            return "Andere";
        default:
            return "Andere";
    }
}

const char* GetPartKey(PartId id) {
    switch (id) {
        case PartId::Trompete1:
            return "trompete_1";
        case PartId::Trompete2:
            return "trompete_2";
        case PartId::Fluegelhorn1:
            return "fluegelhorn_1";
        case PartId::Fluegelhorn2:
            return "fluegelhorn_2";
        case PartId::Gesang:
            return "gesang";
        case PartId::Andere:
            return "andere";
        default:
            return "andere";
    }
}

const char* GetPartTitle(PartId id) {
    switch (id) {
        case PartId::Trompete1:
            return "1. Trompete";
        case PartId::Trompete2:
            return "2. Trompete";
        case PartId::Fluegelhorn1:
            return "1. Fluegelhorn";
        case PartId::Fluegelhorn2:
            return "2. Fluegelhorn";
        case PartId::Gesang:
            return "Gesang";
        case PartId::Andere:
            return "Andere";
        default:
            return "Andere";
    }
}

}  // namespace no::app
