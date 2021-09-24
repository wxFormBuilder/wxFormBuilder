#
# Regular cron jobs for the wxformbuilder package
#
0 4	* * *	root	[ -x /usr/bin/wxformbuilder_maintenance ] && /usr/bin/wxformbuilder_maintenance
