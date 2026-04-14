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

const char* GetInstrumentKey(InstrumentId id) {
    switch (id) {
        case InstrumentId::Trompete1:
            return "trompete_1";
        case InstrumentId::Trompete2:
            return "trompete_2";
        case InstrumentId::Fluegelhorn1:
            return "fluegelhorn_1";
        case InstrumentId::Fluegelhorn2:
            return "fluegelhorn_2";
        case InstrumentId::Gesang:
            return "gesang";
        case InstrumentId::Andere:
            return "andere";
        default:
            return "andere";
    }
}

const char* GetInstrumentTitle(InstrumentId id) {
    switch (id) {
        case InstrumentId::Trompete1:
            return "1. Trompete";
        case InstrumentId::Trompete2:
            return "2. Trompete";
        case InstrumentId::Fluegelhorn1:
            return "1. Fluegelhorn";
        case InstrumentId::Fluegelhorn2:
            return "2. Fluegelhorn";
        case InstrumentId::Gesang:
            return "Gesang";
        case InstrumentId::Andere:
            return "Andere";
        default:
            return "Andere";
    }
}

}  // namespace no::app
