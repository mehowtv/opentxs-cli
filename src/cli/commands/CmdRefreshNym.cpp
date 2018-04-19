/************************************************************
 *
 *                 OPEN TRANSACTIONS
 *
 *       Financial Cryptography and Digital Cash
 *       Library, Protocol, API, Server, CLI, GUI
 *
 *       -- Anonymous Numbered Accounts.
 *       -- Untraceable Digital Cash.
 *       -- Triple-Signed Receipts.
 *       -- Cheques, Vouchers, Transfers, Inboxes.
 *       -- Basket Currencies, Markets, Payment Plans.
 *       -- Signed, XML, Ricardian-style Contracts.
 *       -- Scripted smart contracts.
 *
 *  EMAIL:
 *  fellowtraveler@opentransactions.org
 *
 *  WEBSITE:
 *  http://www.opentransactions.org/
 *
 *  -----------------------------------------------------
 *
 *   LICENSE:
 *   This Source Code Form is subject to the terms of the
 *   Mozilla Public License, v. 2.0. If a copy of the MPL
 *   was not distributed with this file, You can obtain one
 *   at http://mozilla.org/MPL/2.0/.
 *
 *   DISCLAIMER:
 *   This program is distributed in the hope that it will
 *   be useful, but WITHOUT ANY WARRANTY; without even the
 *   implied warranty of MERCHANTABILITY or FITNESS FOR A
 *   PARTICULAR PURPOSE.  See the Mozilla Public License
 *   for more details.
 *
 ************************************************************/

#include "CmdRefreshNym.hpp"

#include <opentxs/opentxs.hpp>

#include <stdint.h>
#include <ostream>
#include <string>

#define OT_METHOD "opentxs::CmdRefreshNym::"

using namespace opentxs;
using namespace std;

CmdRefreshNym::CmdRefreshNym()
{
    command = "refreshnym";
    args[0] = "--server <server>";
    args[1] = "--mynym <nym>";
    category = catNyms;
    help = "Download mynym's latest intermediary files.";
}

CmdRefreshNym::~CmdRefreshNym() {}

int32_t CmdRefreshNym::runWithOptions()
{
    return run(getOption("server"), getOption("mynym"));
}

int32_t CmdRefreshNym::run(string server, string mynym)
{
    if (!checkServer("server", server)) {
        return -1;
    }

    if (!checkNym("mynym", mynym)) {
        return -1;
    }

    bool msgWasSent = false;
    auto nymBox = retrieve_nym(server, mynym, msgWasSent, true);
    switch (nymBox) {
        case 1:
            break;

        case 0:
            if (msgWasSent) {
                otOut << "Error: cannot refresh nym.\n";
                return -1;
            }

            otOut << "Nymbox is empty.\n";
            break;

        default:
            otOut << "Error: cannot refresh nym.\n";
            return -1;
    }

    return 1;
}

// RETRIEVE NYM INTERMEDIARY FILES
// Returns:
//  True if I have enough numbers, or if there was success getting more
// transaction numbers.
//  False if I didn't have enough numbers, tried to get more, and failed
// somehow.
//
std::int32_t CmdRefreshNym::retrieve_nym(
    const std::string& strNotaryID,
    const std::string& strMyNymID,
    bool& bWasMsgSent,
    bool bForceDownload) const
{
    auto context = OT::App().Wallet().mutable_ServerContext(
        Identifier(strMyNymID), Identifier(strNotaryID));
    Utility MsgUtil(context.It(), OT::App().API().OTAPI());

    if (0 >= context.It().UpdateRequestNumber()) {
        otErr << OT_METHOD << __FUNCTION__
              << ": Failed calling getRequestNumber" << std::endl;

        return -1;
    } else  // If it returns 1, we know for sure that the request number is in
            // sync.
    {
        otInfo << "SUCCESS syncronizing the request number." << std::endl;
    }

    std::int32_t nGetAndProcessNymbox = MsgUtil.getAndProcessNymbox_4(
        strNotaryID, strMyNymID, bWasMsgSent, bForceDownload);

    return nGetAndProcessNymbox;
}
