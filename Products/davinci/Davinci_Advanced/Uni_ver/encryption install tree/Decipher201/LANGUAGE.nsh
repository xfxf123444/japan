
!macro MUI_ShowRegistration_SUBTITLE
  !insertmacro MUI_HEADER_TEXT "�����p�ɂȂ���́A�����O�A�����ƁA���i�̃V���A���ԍ�����͂��Ă��������B" " "
!macroend

!macro MUI_showreadme_SUBTITLE
  !insertmacro MUI_HEADER_TEXT "���ǂ݂�������" ""
!macroend

!macro ossystem
  messagebox  MB_ICONINFORMATION|MB_OK "�{���i�́A���̃o�[�W�����̃I�y���[�e�B���O�V�X�e�����T�|�[�g���Ă��܂���B$\r $\n �o�[�W�����A�b�v�ɂ��ẮA�������܂ł��₢���킹���������B"
!macroend

!macro admin
  messageBox MB_OK "���̐��i���C���X�g�[������ɂ́A���̃R���s���[�^�̊Ǘ��҃��[�U�ł���K�v������܂��B�Ǘ��҃��[�U�Ƃ��ă��O�I�����āA���߂āA�C���X�g�[�����s���Ă��������B"
!macroend

!macro serial
  MessageBox MB_ICONSTOP|MB_YESNO "�����ȃV���A���ԍ������͂���܂����B�����𑱂��邽�߂ɂ͐������V���A���ԍ�����͂��Ă��������B�������V���A���ԍ���$\r���͂����ɏ����𑱂��邱�Ƃ͂ł��܂���B$\r $\n�V���A���ԍ�����͂������܂����B." IDno +2
!macroend

!macro shd2
   MessageBox MB_ICONSTOP|MB_YESNO|MB_ICONEXCLAMATION "���̃A�v���P�[�V�����́A���ɃC���X�g�[������Ă��܂�.$\r $\n[�͂�]���N���b�N����ƁA�Â��o�[�W�������A���C���X�g�[�����āA�V�K�ɃC���X�g�[�����܂��B.$\r $\n[������]���N���b�N����ƁA�㏑���ŃC���X�g�[�����܂��B." IDYES tr2 IDNO fa2
!macroend

!macro shd1
   MessageBox MB_ICONSTOP|MB_YESNO|MB_ICONEXCLAMATION "���̃A�v���P�[�V�����́A���ɃC���X�g�[������Ă��܂�.$\r $\n[�͂�]���N���b�N����ƁA�Â��o�[�W�������A���C���X�g�[�����āA�V�K�ɃC���X�g�[�����܂��B.$\r $\n[������]���N���b�N����ƁA�㏑���ŃC���X�g�[�����܂��B." IDYES tr1 IDNO fa1
!macroend

!macro shd
   MessageBox MB_ICONSTOP|MB_YESNO|MB_ICONEXCLAMATION "���̃A�v���P�[�V�����́A���ɃC���X�g�[������Ă��܂�.$\r $\n[�͂�]���N���b�N����ƁA�Â��o�[�W�������A���C���X�g�[�����āA�V�K�ɃC���X�g�[�����܂��B.$\r $\n[������]���N���b�N����ƁA�㏑���ŃC���X�g�[�����܂��B." IDYES tr IDNO fa
!macroend                                              

!macro root
  !define MUI_FINISHPAGE_SHOWREADME_TEXT RebootNow
!macroend

!macro rootnow
 MessageBox MB_YESNO|MB_ICONQUESTION "�R���s���[�^���ċN�����Ă���낵���ł���" IDNO +2
!macroend

!macro beforun
  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "�{���ɁA $(^Name) �ƁA���̊֘A�̃R���|�[�l���g�����S�ɍ폜���܂���?" IDYES +2
!macroend

!macro afterun
  MessageBox MB_ICONINFORMATION|MB_OK "$(^Name) �̓V�X�e�������菜����܂����B�A���C���X�g�[�������̍ۂɎg�p���������t�@�C�����m���ɍ폜���邽�߂ɁA�}�V�����ċN�����Ă��������B"
!macroend





