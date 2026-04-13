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
enum class PartId {
    Trompete1 = 0,
    Trompete2,     //
    Fluegelhorn1,  //
    Fluegelhorn2,  //
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
const char* GetPartKey(PartId id);

/**
 * \brief Returns the display title for a part.
 */
const char* GetPartTitle(PartId id);

}  // namespace no::app
