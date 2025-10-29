#include "DolphinQt/WiiMix/QueryBuilder.h"

#ifndef SERVER_ACTION
    // Relevant parameters
    #define SERVER_ACTION "ACTION"
    #define SELECT "SELECT"
    #define WHERE "WHERE"
    #define FILTER_GREATER_THAN "FILTER_GREATER_THAN"
    #define FILTER_LESS_THAN "FILTER_LESS_THAN"
    #define SORT_ASC "SORT_ASC"
    #define SORT_DESC "SORT_DESC"
    #define NUM_ROWS "NUM_ROWS"
    #define COUNT "COUNT"
#endif

// --- Constructor ---
QueryBuilder::QueryBuilder(WiiMixEnums::Action action, WiiMixEnums::Response response) {
    m_json[QStringLiteral(SERVER_ACTION)] = static_cast<int>(action);
    m_json[QStringLiteral(CLIENT_RESPONSE)] = static_cast<int>(response);
}

// --- Builder Methods ---
QueryBuilder& QueryBuilder::selectWhere(const QString& column, const QJsonValue& value) {
    m_json[QStringLiteral(SELECT)] = column;
    m_json[QStringLiteral(WHERE)] = value;
    return *this;
}

QueryBuilder& QueryBuilder::filterGreaterThan(const QString& column, const QJsonValue& value) {
    // Creates or appends to the "FILTER_GREATER_THAN" sub-object
    QJsonObject filters = m_json[QStringLiteral(FILTER_GREATER_THAN)].toObject();
    filters[column] = value;
    m_json[QStringLiteral(FILTER_GREATER_THAN)] = filters;
    return *this;
}

QueryBuilder& QueryBuilder::filterLessThan(const QString& column, const QJsonValue& value) {
    // Creates or appends to the "FILTER_LESS_THAN" sub-object
    QJsonObject filters = m_json[QStringLiteral(FILTER_LESS_THAN)].toObject();
    filters[column] = value;
    m_json[QStringLiteral(FILTER_LESS_THAN)] = filters;
    return *this;
}

QueryBuilder& QueryBuilder::filterIncludes(const QString& column, const QJsonValue& value) {
    // Creates or appends to the "FILTER_INCLUDES" sub-object
    QJsonObject filters = m_json[QStringLiteral(FILTER_INCLUDES)].toObject();
    filters[column] = value;
    m_json[QStringLiteral(FILTER_INCLUDES)] = filters;
    return *this;
}

QueryBuilder& QueryBuilder::filterOrIncludes(const QString& column, const QJsonValue& value) {
    // Creates or appends to the "FILTER_INCLUDES" sub-object
    QJsonObject filters = m_json[QStringLiteral(FILTER_OR_INCLUDES)].toObject();
    filters[column] = value;
    m_json[QStringLiteral(FILTER_OR_INCLUDES)] = filters;
    return *this;
}

QueryBuilder& QueryBuilder::sortByAsc(const QString& column) {
    // Creates or appends to the "SORT_ASC" array
    QJsonArray sortKeys = m_json[QStringLiteral(SORT_ASC)].toArray();
    sortKeys.append(column);
    m_json[QStringLiteral(SORT_ASC)] = sortKeys;
    return *this;
}

QueryBuilder& QueryBuilder::sortByDesc(const QString& column) {
    // Creates or appends to the "SORT_DESC" array
    QJsonArray sortKeys = m_json[QStringLiteral(SORT_DESC)].toArray();
    sortKeys.append(column);
    m_json[QStringLiteral(SORT_DESC)] = sortKeys;
    return *this;
}

QueryBuilder& QueryBuilder::limit(int count) {
    m_json[QStringLiteral(LIMIT_NUM_ROWS)] = count;
    return *this;
}

QueryBuilder& QueryBuilder::asCount() {
    m_json[QStringLiteral(COUNT_NUM_ROWS)] = true;
    return *this;
}

QueryBuilder& QueryBuilder::with(const QString& key, const QJsonValue& value) {
    m_json[key] = value;
    return *this;
}

QueryBuilder& QueryBuilder::random() {
    m_json[QStringLiteral(RANDOM)] = true;
    return *this;
}

// --- Finalizer ---
QJsonObject QueryBuilder::build() const {
    return m_json;
}