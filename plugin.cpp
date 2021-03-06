/**
 * Copyright (c) 2018 - Marcos Cardinot <marcos@cardinot.net>
 * Copyright (c) 2018 - Ethan Padden <e.padden1@nuigalway.ie>
 *
 * This source code is licensed under the MIT license found in
 * the LICENSE file in the root directory of this source tree.
 */

#include "plugin.h"

namespace evoplex {

QBitArray LifeLike::ruleToBitArray(int cmd) const
{

    QBitArray rules(9);
    
    // convert rule to a bitstream 
    
    // Check the command should be empty (input of -1)
    // before iterating.
    if (cmd != -1)
    {
        int c;
        
        do
        { 
            c = cmd%10;
            
            if (c == 9) {
                qWarning() << "Command should contain only valid integers (0-8).";
                return QBitArray();
            }
            if (rules[c]){
                qWarning() << "Rulestring should contain only unique integers.";
                return QBitArray();
            }
            rules.setBit(c);
            cmd /= 10;
        } while(cmd);
    }
    
    return rules;
}

bool LifeLike::init()
{
    // gets the id of the `live` node's attribute, which is the same for all nodes
    m_liveAttrId = node(0).attrs().indexOf("live");
   
    // parses the ruleset    
    if (attrExists("birth") && attrExists("survival")) {
        m_birthLst = ruleToBitArray(attr("birth").toInt());
        m_survivalLst = ruleToBitArray(attr("survival").toInt());
    } else{
        qWarning() << "missing attributes.";
        return false;
    }

    return m_liveAttrId >= 0 && !m_birthLst.isNull() && !m_survivalLst.isNull();
}

bool LifeLike::algorithmStep()
{
    std::vector<Value> nextStates;
    nextStates.reserve(nodes().size());

    for (Node node : nodes()) {
        int liveNeighbourCount = 0;
        for (Node neighbour : node.outEdges()) {
            if (neighbour.attr(m_liveAttrId).toBool()) {
                ++liveNeighbourCount;
            }
        }

        if (node.attr(m_liveAttrId).toBool()) {
            // If the node is alive, then it only survives if its number of neighbors is specified in the rulestring.
            // Otherwise, it dies from under/overpopulation
               nextStates.emplace_back(m_survivalLst[liveNeighbourCount]);
        } else {
            // Any dead cell can become alive if its number of neighbors matches the one specified in the rulestring.
            // Otherwise, it remains dead.
            nextStates.emplace_back(m_birthLst[liveNeighbourCount]);
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
