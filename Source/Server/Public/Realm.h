#pragma once

namespace Battlenet
{
	struct /*SHARED_API*/ RealmHandle
	{
		RealmHandle() :
			Region(0), Site(0), Realm(0) { }

		RealmHandle(uint8 region, uint8 battlegroup, uint32 index) :
			Region(region), Site(battlegroup), Realm(index) { }

		RealmHandle(uint32 realmAddress) :
			Region((realmAddress >> 24) & 0xFF), Site((realmAddress >> 16) & 0xFF), Realm(realmAddress & 0xFFFF) { }

		uint8 Region;
		uint8 Site;
		uint32 Realm;   // primary key in `realmlist` table

		bool operator<(RealmHandle const& r) const
		{
			return Realm < r.Realm;
		}

		uint32 GetAddress() const { return (Region << 24) | (Site << 16) | uint16(Realm); }
		std::string GetAddressString() const { return String::Format("{0}-{1}-{2}", Region, Site, Realm); }
		std::string GetSubRegionAddress() const { return String::Format("{0}-{1}-0", Region, Site, Realm); }
	};
}