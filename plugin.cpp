/**
 * Copyright (c) 2018 - Marcos Cardinot <marcos@cardinot.net>
 * Copyright (c) 2018 - Ethan Padden <e.padden1@nuigalway.ie>
 *
 * This source code is licensed under the MIT license found in
 * the LICENSE file in the root directory of this source tree.
 */

#include "plugin.h"

namespace evoplex {

QBitArray LifeLike::parseCmd(const QString &cmd1, const QString &cmd2)
{
    QString _cmd1 = cmd1;
    QString _cmd2 = cmd2;
    QBitArray rules(18);

    int intRule1 = _cmd1.toInt();
    int intRule2 = _cmd2.toInt();
    
    // convert rule to a bitstream 
    
    // Check if either of the commands should be empty (input of -1)
    // before iterating.
    if (intRule1 != -1)
    {
        for (const auto& c : _cmd1)
        {
            if (rules[c.digitValue() + 0x0A]){
                qWarning() << "Rulestring should contain only unique integers.";
                return QBitArray();
            }
            rules.setBit(c.digitValue() + 0x0A);
        }
    }
    
    if (intRule2 != -1)
    {
        for (const auto& c: _cmd2)
        {
            if (rules[c.digitValue()]){
                qWarning() << "Rulestring should contain only unique integers.";
                return QBitArray();
            }
            rules.setBit(c.digitValue());
        }
    }
    return rules;
}

bool LifeLike::init()
{
    // gets the id of the `live` node's attribute, which is the same for all nodes
    m_liveAttrId = node(0).attrs().indexOf("live");
   
    // parses the ruleset    
    if (attrExists("birth") && attrExists("survival")){
        m_birth = attr("birth").toQString();
        m_survival = attr("survival").toQString();
    }
    else{
        qWarning() << "missing attributes.";
        return false;
    }
    
    m_rulesetLst = parseCmd(m_birth, m_survival);
    
    if (m_rulesetLst.isNull())
    {
        return false;
    }

    return m_liveAttrId >= 0;
}

bool LifeLike::algorithmStep()
{
    std::vector<Value> nextStates;
    nextStates.reserve(nodes().size());

    for (Node node : nodes()) {
        int liveNeighbourCount = 0;
        for (Node neighbour : node.outEdges()){
            if (neighbour.attr(m_liveAttrId).toBool()) {
                ++liveNeighbourCount;
            }
        }

        if (node.attr(m_liveAttrId).toBool()) {
            // If the node is alive, then it only survives if its number of neighbors is specified in the rulestring.
            // Otherwise, it dies from under/overpopulation
               nextStates.emplace_back(m_rulesetLst[liveNeighbourCount]);
        } else {
            // Any dead cell can become alive if its number of neighbors matches the one specified in the rulestring.
            // Otherwise, it remains dead.
            nextStates.emplace_back(m_rulesetLst[liveNeighbourCount + 0x0A]);
        }
    }
    // For each node, load the next state into the current state
    size_t i = 0;
    for (Node node : nodes()) {
        node.setAttr(m_liveAttrId, nextStates.at(i));
        ++i;
    }
    return true;
}

} // evoplex
REGISTER_PLUGIN(LifeLike)
#include "plugin.moc"
