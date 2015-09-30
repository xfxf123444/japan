#ifndef CRYPTOPP_OIDS_H
#define CRYPTOPP_OIDS_H

// common ASN.1 object identifiers

#include "asn.h"

NAMESPACE_BEGIN(CryptoPP)

NAMESPACE_BEGIN(ASN1)

#define DEFINE_OID(value, name)	inline OID name() {return value;}

DEFINE_OID(1, iso)
	DEFINE_OID(iso()+2, member_body)
		DEFINE_OID(member_body()+840, iso_us)
			DEFINE_OID(iso_us()+10040, ansi_x9_57)
				DEFINE_OID(ansi_x9_57()+4+1, id_dsa)
			DEFINE_OID(iso_us()+10045, ansi_x9_62)
				DEFINE_OID(ansi_x9_62()+1, id_fieldType)
					DEFINE_OID(id_fieldType()+1, prime_field)
					DEFINE_OID(id_fieldType()+2, characteristic_two_field)
						DEFINE_OID(characteristic_two_field()+3, id_characteristic_two_basis)
							DEFINE_OID(id_characteristic_two_basis()+1, gnBasis)
							DEFINE_OID(id_characteristic_two_basis()+2, tpBasis)
							DEFINE_OID(id_characteristic_two_basis()+3, ppBasis)
				DEFINE_OID(ansi_x9_62()+2, id_publicKeyType)
					DEFINE_OID(id_publicKeyType()+1, id_ecPublicKey)
			DEFINE_OID(iso_us()+113549, rsadsi)
				DEFINE_OID(rsadsi()+1, pkcs)
					DEFINE_OID(pkcs()+1, pkcs_1)
						DEFINE_OID(pkcs_1()+1, rsaEncryption);
DEFINE_OID(2, joint_iso_ccitt)
	DEFINE_OID(joint_iso_ccitt()+16, country)
		DEFINE_OID(country()+840, joint_iso_ccitt_us)
			DEFINE_OID(joint_iso_ccitt_us()+1, us_organization)
				DEFINE_OID(us_organization()+101, us_gov)
					DEFINE_OID(us_gov()+3, csor)
						DEFINE_OID(csor()+4, nistalgorithms)
							DEFINE_OID(nistalgorithms()+1, aes)
								DEFINE_OID(aes()+1, id_aes128_ECB)
								DEFINE_OID(aes()+2, id_aes128_cbc)
								DEFINE_OID(aes()+3, id_aes128_ofb)
								DEFINE_OID(aes()+4, id_aes128_cfb)
								DEFINE_OID(aes()+21, id_aes192_ECB)
								DEFINE_OID(aes()+22, id_aes192_cbc)
								DEFINE_OID(aes()+23, id_aes192_ofb)
								DEFINE_OID(aes()+24, id_aes192_cfb)
								DEFINE_OID(aes()+41, id_aes256_ECB)
								DEFINE_OID(aes()+42, id_aes256_cbc)
								DEFINE_OID(aes()+43, id_aes256_ofb)
								DEFINE_OID(aes()+44, id_aes256_cfb)

NAMESPACE_END

NAMESPACE_END

#endif
