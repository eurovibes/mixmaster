# mix.cfg - Mixmaster configuration file

####################### Remailer configuration: ###########################
REMAIL		y

SHORTNAME	foo

REMAILERADDR	mix@remailer.invalid
#ANONADDR	nobody@remailer.invalid
#COMPLAINTS	abuse@remailer.invalid

REMAILERNAME	Anonymous Remailer
ANONNAME	Anonymous

# Supported formats:
MIX		y
PGP		n
UNENCRYPTED	n

# Filter binary attachments?
BINFILTER	y

# Maximum message size in kB:
SIZELIMIT	0

POOLSIZE	20
RATE		95
SENDPOOL	1h
INDUMMYP	20
OUTDUMMYP	67
IDEXP		7d
PACKETEXP	7d

SENDMAIL	/usr/lib/sendmail -t

# Where to log error messages:
ERRLOG		error.log

# Path to inews, or address of mail-to-news gateway:
#NEWS		mail2news@nym.alias.net

# Anti-spam message IDs on Usenet (MD5 of message body)?
MID		y

ORGANIZATION	Anonymous Posting Service

######################## Client configuration: ##########################

NUMCOPIES	1
CHAIN		*,*,*,*
DISTANCE	2
MINREL		98
RELFINAL	99
MAXLAT		36h
