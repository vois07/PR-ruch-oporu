
namespace utils

{

	enum TAGS {invitation, inviYes,inviNo, permissionAsk, permissionYes, permissionNo, resourceAsk, resourceYes, resourceNo, meetingStart, meetingAttend, meetingUpdate, meetingEnd};

	enum CHANS{ acceptorCh};
	int getMeetingChannel(int);
	void makeMPI_UPDATE();
};
