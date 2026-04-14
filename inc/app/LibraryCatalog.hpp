#pragma once

namespace no::app {

/**
 * \brief Supported top-level music books.
 */
enum class BookId {
    Marschbuch = 0,
    Kirchenbuch,    //
    MareinerBlech,  //
    Quartett,       //
    BrassEnsemble,  //
    Duette,         //
    Lieder,         //
    Andere
};

/**
 * \brief Supported musical parts.
 */
enum class InstrumentId {
    Trompete1 = 0,
    Trompete2,     //
    Trompete3,     //
    Fluegelhorn1,  //
    Fluegelhorn2,  //
    Fluegelhorn3,  //
    Gesang,        //
    Andere
};

/**
 * \brief Returns the stable string identifier for a book.
 */
const char* GetBookKey(BookId id);

/**
 * \brief Returns the display title for a book.
 */
const char* GetBookTitle(BookId id);

/**
 * \brief Returns the stable string identifier for a part.
 */
const char* GetInstrumentKey(InstrumentId id);

/**
 * \brief Returns the display title for a part.
 */
const char* GetInstrumentTitle(InstrumentId id);

}  // namespace no::app
