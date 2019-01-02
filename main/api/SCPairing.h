/* Smart Chair Embedded Controller
 * Pairing Controller Header
 *
 * Team Smart Chair - UAH CPE496 2019
*/

#ifndef SCPAIRING_H
#define SCPAIRING_H

#include <string>

#include "webapi.h"

class SCPairing
{
public:
	bool BeginPairing();
	bool PollPairingStatus();
	bool FinishPairing();

	bool HasPairingBegun();
	bool IsPairingComplete();
	std::string GetPairingKey();
	std::string GetAuthKey();

private:
	std::string pairingKey;
	std::string authKey;

	static constexpr const char* TAG = "pairing";
};

#endif /*SCPAIRING_H*/
