/**
 * Copyright (c) 2018 - Marcos Cardinot <marcos@cardinot.net>
 * Copyright (c) 2018 - Ethan Padden <e.padden1@nuigalway.ie>
 *
 * This source code is licensed under the MIT license found in
 * the LICENSE file in the root directory of this source tree.
 */

#ifndef LIFELIKE_H
#define LIFELIKE_H

#include <QBitArray>
 
#include <plugininterface.h>

namespace evoplex {
class LifeLike: public AbstractModel
{
public:
    bool init() override;
    bool algorithmStep() override;
    QBitArray parseCmd(const QString &cmd);

private:
    int m_liveAttrId;  // the id of the 'live' node's attribute
    QString m_ruleset; // the model's ruleset (B/S format)
    QBitArray m_rulesetLst; // the bitstream representing the input ruleset
};
} // evoplex
#endif // LIFELIKE_H
