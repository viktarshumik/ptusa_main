/// @file PAC_dev.hh 
/// @brief ������ � �������, ��������� � Lua.
///
/// @par ������� ������:
/// @$Rev$.\n
/// @$Author$.\n
/// @$Date::                     $.

$#include <stdlib.h>

$#include "PAC_dev.h"
$#include "tech_def.h"
$#include "cip_tech_def.h"
$#include "wago.h"
$#include "tolua_base.h"
$#include "PID.h"
$#include "g_device.h"
$#include "errors.h"

$#include "rm_manager.h"
$#include "modbus_client.h"

//-----------------------------------------------------------------------------
/// @brief ���������� �� ������ ����������� �����.
///
/// �������� �����, ���������� ������� � �.�. �������� ��������� �����
/// ���������.
class i_DI_device
    {
    public:
        /// @brief ��������� ���������������� ��������� ����������.
        ///
        /// ���������� ������ ���� ���������, ���� ���������� ���� ��������� �
        /// � ������ ��������� ������ ��������� ��������� �������.
        ///
        /// @return - ��������� ���������� � ���� ������ �����.
        int get_state();

        /// @brief ��������� ����������� ��������� ��������� ����������.
        bool is_active();
    };
//-----------------------------------------------------------------------------
/// @brief ���������� �� ������ ����������� ������.
///
/// ������, ������� � �.�. �������� ��������� ����� ���������.
class i_DO_device: public i_DI_device
    {
    public:
        /// @brief ��������� ����������.
        ///
        /// ��������� ���������� � �������� ���������. ��� ������� ��� ��������
        /// ��� �������������, �� ���� ���� �� ��������� �������� - ��������.
        void on();

        /// @brief ���������� ����������.
        ///
        /// ��������� ���������� � ��������� ���������. ��� ������� ��� ��������
        /// ��� ���������������, �� ���� ���� �� ��������� �������� - ��������.
        void off();

        /// @brief ��������� ������ ��������� ����������.
        ///
        /// @param new_state - ����� ��������� �������.
        void set_state( int new_state );
    };
//-----------------------------------------------------------------------------
/// @brief ���������� �� �� ������ ����������� �����.
///
/// �����������, ������ � �.�. �������� ��������� ����� ���������.
class i_AI_device
    {
    public:
        /// @brief ��������� �������� ��������� ����������.
        ///
        /// @return - ������� ��������� ���������� � ���� �������� �����.
        float get_value();
    };
//-----------------------------------------------------------------------------
/// @brief ���������� �� ������ ����������� ������.
///
/// ���������� ����� ���������� � �.�. �������� ��������� ����� ���������.
class i_AO_device: public i_AI_device
    {
    public:
        /// @brief ���������� ����������.
        ///
        /// ��������� ���������� � ��������� ���������. ��� ������� ��� ��������
        /// ��� ���������������, �� ���� ���� �� ��������� �������� - ��������.
        void off();

        /// @brief ��������� �������� ��������� ����������.
        ///
        /// @param new_value - ����� ��������� ����������.
        void set_value( float new_value );
    };
//-----------------------------------------------------------------------------
/// @brief ��������� ���������� ��� � �����������, ��� � ����������� ��������.
class i_DO_AO_device: public i_DO_device, public i_AO_device
    {
	public:
        /// @brief ��������� ���������������� ��������� ����������.
        ///
        /// ���������� ������ ���� ���������, ���� ���������� ���� ��������� �
        /// � ������ ��������� ������ ��������� ��������� �������.
        ///
        /// @return - ��������� ���������� � ���� ������ �����.
        int get_state();

        /// @brief ��������� ���������.
        ///
        /// ������ ����� ������������ ��� ������� ��������� ���������� �����
        /// ��� ���������.
        ///
        /// @param new_state - ����� ���������.
        void set_state( int new_state );

		/// @brief ��������� ����������.
        ///
        /// ��������� ���������� � �������� ���������. ��� ������� ��� ��������
        /// ��� �������������, �� ���� ���� �� ��������� �������� - ��������.
        void on();

        /// @brief ���������� ����������.
        ///
        /// ��������� ���������� � ��������� ���������. ��� ������� ��� ��������
        /// ��� ���������������, �� ���� ���� �� ��������� �������� - ��������.
        void off();

        /// @brief ��������� ������ ��������� ����������.
        ///
        /// @param new_state - ����� ��������� �������.
        void set_state( int new_state );



	    /// @brief ��������� �������� ��������� ����������.
        ///
        /// @return - ������� ��������� ���������� � ���� �������� �����.
        float get_value();

        /// @brief ���������� ����������.
        ///
        /// ��������� ���������� � ��������� ���������. ��� ������� ��� ��������
        /// ��� ���������������, �� ���� ���� �� ��������� �������� - ��������.
        void off();

        /// @brief ��������� �������� ��������� ����������.
        ///
        /// @param new_value - ����� ��������� ����������.
        void set_value( float new_value );
    };
//-----------------------------------------------------------------------------
/// @brief ��������� ��������.
class i_counter
    {
    public:
        /// @brief ������������ ������ ��������.
        void pause();

        /// @brief ������������� ������ ��������.
        void start();

        /// @brief ����� ��������.
        ///
        /// ����� ������ ��� ����������� ������ ���������� ������� @ref start().
        void reset();

        /// @brief ����� �������� � ����������� �����.
        void restart();

        /// @brief ��������� �������� �������� (�����).
        unsigned long get_quantity();

		/// @brief ��������� �������� �������� (�����).
		float get_flow();

        /// @brief ��������� ��������� ������ ��������.
        virtual int get_state() = 0;
    };
//-----------------------------------------------------------------------------
/// @brief ������� ���������� ����������.
///
/// ������� ����� ���������: ������, �����, ������� � �.�.
class device
    {
    public:
        /// @brief ���������� ������� �����������.
        ///
        /// ��� ��������� ������, ���������� �� �������.
        int set_cmd( const char *prop, unsigned int idx, double val );

		void set_par( unsigned int idx, unsigned int offset, float value );
				
		void set_property( const char* field, device* dev );

		void set_descr( const char *description );

		enum DEVICE_TYPE
            {
            DT_NONE = -1,///< ��� �� ���������.

            DT_V = 0,   ///< ������. 
            DT_VC,      ///< ����������� ������. 
            DT_M,       ///< ���������.
            DT_LS,      ///< ������� (����/���).
            DT_TE,      ///< �����������.        
            DT_FS,      ///< ������ (����/���).
            DT_GS,      ///< ������ ���������. 
            DT_FQT,     ///< �������.        
            DT_LT,      ///< ������� (��������).        
            DT_QT,      ///< ������������.

            DT_HA,      ///< ��������� �������� ������������.
            DT_HL,      ///< ��������� �������� ������������.
            DT_SB,      ///< ������.
            DT_DI,      ///< ���������� ������� ������.
            DT_DO,      ///< ���������� �������� ������.
            DT_AI,      ///< ���������� ������� ������.
            DT_AO,      ///< ���������� �������� ������.
            };
    };
//-----------------------------------------------------------------------------
/// @brief ��������� ������� �� �����.
///
/// @param dev_name - ��� �������.
/// @return - ������ � �������� ������. ���� ��� ������ �������, ������������
/// �������� (@ref dev_stub).
i_DO_device* V( const char *dev_name );
//-----------------------------------------------------------------------------
/// @brief ��������� ������������ ������� �� �����.
///
/// @param dev_name - ��� �������.
/// @return - ������ � �������� ������. ���� ��� ������ �������, ������������
/// �������� (@ref dev_stub).
i_AO_device* VC( const char *dev_name );
//-----------------------------------------------------------------------------
/// @brief ��������� ��������� �� �����.
///
/// @param dev_name - ��� ���������.
/// @return - ��������� � �������� ������. ���� ��� ������ ����������,
/// ������������ �������� (@ref dev_stub).
i_DO_AO_device* M( const char *dev_name );
//-----------------------------------------------------------------------------
/// @brief ��������� ����������� ������ �� �����.
///
/// @param dev_name - ��� ����������� ������.
/// @return - ���������� � �������� ������. ���� ��� ������ ����������,
/// ������������ �������� (@ref dev_stub).
i_DI_device* LS( const char *dev_name );
//-----------------------------------------------------------------------------
/// @brief ��������� ����������� ������� �� �����.
///
/// @param dev_name - ��� ����������� �������.
/// @return - ���������� � �������� ������. ���� ��� ������ ����������,
/// ������������ �������� (@ref dev_stub).
i_DI_device* FS( const char *dev_name );
//-----------------------------------------------------------------------------
/// @brief ��������� ����������� ����� �� �����.
///
/// @param dev_name - ��� ����������� �����.
/// @return - ���������� � �������� ������. ���� ��� ������ ����������,
/// ������������ �������� (@ref dev_stub).
i_AI_device* AI( const char *dev_name );
//-----------------------------------------------------------------------------
/// @brief ��������� ����������� ������ �� �����.
///
/// @param dev_name - ��� ����������� ������.
/// @return - ���������� � �������� ������. ���� ��� ������ ����������,
/// ������������ �������� (@ref dev_stub).
i_AO_device* AO( const char *dev_name );
//-----------------------------------------------------------------------------
/// @brief ��������� �������� �� �����.
///
/// @param dev_name - ��� ��������.
/// @return - ���������� � �������� �������. ���� ��� ������ ����������,
/// ������������ �������� (@ref dev_stub).
i_counter* FQT( const char *dev_name );
//-----------------------------------------------------------------------------
/// @brief ��������� ����������� �� �����.
///
/// @param dev_name - ��� �����������.
/// @return - ���������� � �������� ������. ���� ��� ������ ����������,
/// ������������ �������� (@ref dev_stub).
i_AI_device* TE( const char *dev_name );
//-----------------------------------------------------------------------------
/// @brief ��������� �������� ������ �� �����.
///
/// @param dev_name - ��� �������� ������.
/// @return - ���������� � �������� ������. ���� ��� ������ ����������,
/// ������������ �������� (@ref dev_stub).
i_AI_device* LT( const char *dev_name );
//-----------------------------------------------------------------------------
/// @brief ��������� ������� ��������� �� �����.
///
/// @param dev_name - ��� ������� ���������.
/// @return - ���������� � �������� ������. ���� ��� ������ ����������,
/// ������������ �������� (@ref dev_stub).
i_DI_device* GS( const char *dev_name );
//-----------------------------------------------------------------------------
/// @brief ��������� �������� ������������ �� �����.
///
/// @param dev_name - ��� ��������� ������������.
/// @return - ���������� � �������� ������. ���� ��� ������ ����������,
/// ������������ �������� (@ref dev_stub).
i_DO_device* HA( const char *dev_name );
//-----------------------------------------------------------------------------
/// @brief ��������� �������� ������������ �� �����.
///
/// @param dev_name - ��� �������� ������������.
/// @return - ���������� � �������� ������. ���� ��� ������ ����������,
/// ������������ �������� (@ref dev_stub).
i_DO_device* HL( const char *dev_name );
//-----------------------------------------------------------------------------
/// @brief ��������� ������ �� �����.
///
/// @param dev_name - ��� ������.
/// @return - ���������� � �������� ������. ���� ��� ������ ����������,
/// ������������ �������� (@ref dev_stub).
i_DI_device* SB( const char *dev_name );
//-----------------------------------------------------------------------------
/// @brief ��������� �������� ����� �� �����.
///
/// @param dev_name - ��� �������� �����.
/// @return - ���������� � �������� ������. ���� ��� ������ ����������,
/// ������������ �������� (@ref dev_stub).
i_DI_device* DI( const char *dev_name );
//-----------------------------------------------------------------------------
/// @brief ��������� ������ ���������� �� �����.
///
/// @param dev_name - ��� ������ ����������.
/// @return - ���������� � �������� ������. ���� ��� ������ ����������,
/// ������������ �������� (@ref dev_stub).
i_DO_device* DO( const char *dev_name );
//-----------------------------------------------------------------------------
/// @brief ��������� ������� ������������ �� �����.
///
/// @param dev_name - ��� ������� ������������.
/// @return - ���������� � �������� ������. ���� ��� ������ ����������,
/// ������������ �������� (@ref dev_stub).
i_AI_device* QT( const char *dev_name );
//-----------------------------------------------------------------------------
/// @brief ��������� ������ �� ������.
///
/// @return - ����������� ����������.
dev_stub* STUB();

device* DEVICE( int s_number );
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/// @brief ����������� ����������.
///
/// ���������� ��� ����������� ���������� ������ ���������� � ��������������
/// �������. ������ ������� ������ ������ �� ������.
class dev_stub
    {
    public:
        /// @brief ��������� ��������� ����������.
        ///
        /// @return 0
        float get_value();

        /// @brief ��������� ��������� ����������. ������ �� ������.
        void set_value( float new_value );


        /// @brief ��������� ����������. ������ �� ������.
        void on();

        /// @brief ���������� ����������. ������ �� ������.
        void off();


        /// @brief ��������� ��������� ����������. ������ �� ������.
        void set_state( int new_state );

        /// @brief ��������� ��������� ����������.
        ///
        /// @return 0
        int get_state();

        /// @brief ��������� ��������. ������ �� ������.
        void pause();

        /// @brief ������������� ��������. ������ �� ������.
        void start();

        /// @brief ����� ��������. ������ �� ������.
        void reset();

        /// @brief ��������� �������� ��������.
        ///
        /// @return 0
        unsigned int get_quantity();
    };
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/// @brief �������� ���������� �� ���� ������� ������� ���������.
class dev_errors_manager
    {
    public:
	    /// @brief ��������� ������������� ���������� ������.
        static dev_errors_manager* get_instance();

        /// @brief ��������� ���������� ������.
        void set_cmd( unsigned int cmd, unsigned int object_type, 
            unsigned int object_number, unsigned int object_alarm_number );
	}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/// @brief ��������� mixproof.
class i_mix_proof 
    {
    public:
        enum STATES
            {
            ST_CLOSE = 0,   ///< ������.
            ST_OPEN,        ///< ������.
            ST_UPPER_SEAT,  ///< ������� ������� �����.
            ST_LOWER_SEAT,  ///< ������� ������ �����.
            };      
    };
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/// @brief ������ � ��������������� ��������.
///
/// ������� ����� ��� ���������������� ������� (�����, ��������). ��������
/// �������� ������ ������ - ������ � �������� � �.�.
class tech_object
    {
    public:
        int set_cmd( const char *prop, unsigned int idx, double val );

        /// @param name                     - �������� ("��������", ...).
        /// @param number                   - �����.
        /// @param type                     - ���.
        /// @param name_Lua                 - ��� � Lua.
        /// @param states_count             - ���������� �������.
        /// @param timers_count             - ���������� ��������.
        /// @param par_float_count          - ���������� ����������� ���������� ���� float.
        /// @param runtime_par_float_count  - ���������� ������� ���������� ���� float.
        /// @param par_uint_count           - ���������� ����������� ���������� ���� uint.
        /// @param runtime_par_uint_count   - ���������� ������� ���������� ���� uint.
        tech_object( const char* name, unsigned int number, unsigned int type,
            const char* name_Lua,
            unsigned int  states_count,
            unsigned int  timers_count,
            unsigned int  par_float_count, unsigned int  runtime_par_float_count,
            unsigned int  par_uint_count, unsigned int runtime_par_uint_count );

        ~tech_object();

        /// @brief ���������/���������� ������.
        ///
        /// @param mode      - �����.
        /// @param new_state - ����� ��������� ������. ��������� ��������: 0 -
        /// ���������, 1 - ��������, -1 - ��������� ��� �������� �� �����������
        ///	����������.
        int set_mode( unsigned int mode, int new_state );

        /// @brief ��������� ��������� ������.
        ///
        /// @param mode - �����.
        ///
        /// @return 1 - ����� �������.
        /// @return 0 - ����� �� �������.
        int get_mode( unsigned int mode );

        /// @brief ���������� �������.
        ///
        /// ����� ����� ����������� �����-���� �������� (����������/�����������
        /// ������ ������, ����������/����������� �����-���� ����������).
        int exec_cmd( unsigned int cmd );

        /// @brief ��������� ����� ������� ���������������� �������.
        ///
        /// ����� ����� ����������� �����-���� �������� (����������/�����������
        /// ������ ������, ����������/����������� �����-���� ����������).
        unsigned int get_modes_count() const;

        saved_params_float      par_float;   ///< ����������� ���-��, ��� float.
        run_time_params_float   rt_par_float;///< ������� ���������, ��� float.
        saved_params_u_int_4    par_uint;    ///< ����������� ���-��, ��� uint.
        run_time_params_u_int_4 rt_par_uint; ///< ������� ���-��, ��� uint.

        timer_manager           timers;		 ///< ������� �������.

        mode_manager* get_modes_manager();

        /// @brief ������ ���������� ������������� �������.
        ///
        bool is_idle() const;

        enum ERR_MSG_TYPES
            {
            ERR_CANT_ON,
            ERR_ON_WITH_ERRORS,
            ERR_OFF,
            ERR_OFF_AND_ON,
            ERR_DURING_WORK,
            ERR_ALARM,
            };

        int set_err_msg( const char *err_msg, int mode, int new_mode = 0, 
            ERR_MSG_TYPES type = ERR_CANT_ON );
    };
//-----------------------------------------------------------------------------
///@brief ��������� ��������� ��������������� ��������.
///
///@return �������� ��������������� �������� �������.
tech_object_manager* G_TECH_OBJECT_MNGR();
//-----------------------------------------------------------------------------
/// @brief �������� ��������������� ��������.
///
/// �������� ���������� ��� ���� ��������������� �������� �������.
class tech_object_manager
    {
    public:
        /// @brief ��������� ������� � �������� �������� �������.
        int get_object_with_active_mode( unsigned int mode,
            unsigned int start_idx, unsigned int end_idx );

        /// @brief ��������� ���������������� ������� �� ��� ����������� ������.
        tech_object* get_tech_objects( unsigned int idx );

        /// @brief ���������� ������ �������.
        void print();
    };
//-----------------------------------------------------------------------------
/// @brief �������� ���������� � ���� ������� ������-���� ������� (����, 
/// ��������).
/// 
/// � ������� (����, ...) ����� ���� �������� ����������� ��������� �������.
class mode_manager
    {    
    public:
        mode* add_mode( const char* name );

		/// @brief ��������� ������ ����� �������� ��������������.
        ///
        /// @param idx - ������ ������.
        ///
        /// @return - �������� ������ � �������� ��������. ���� ������
        /// ������� �� ��������, ������������ �������� �������� - ����
        /// mode_stub.
        mode* operator[] ( unsigned int idx );

		/// @brief ����� ����������� (��� ���������� �������).
        ///
        /// @return - ����� ������� ��� �������� �������.
        unsigned long get_idle_time();
    };
//-----------------------------------------------------------------------------
class mode
    {    
	public:
		unsigned long evaluation_time();

        step* add_step( const char* name, unsigned int next_step_n,
            unsigned int step_duration_par_n );

        /// @brief ��������� ������ ��������� �� �������� ����������� ������������ �����.
        void set_step_cooperate_time_par_n( int step_cooperate_time_par_n );

		unsigned int active_step() const;

        unsigned long active_step_evaluation_time() const;

        /// @brief ��������� ������ ����� �������� ��������������.
        ///
        /// @param idx - ������ ������.
        ///
        /// @return - �������� ������ � �������� ��������. ���� ������
        /// ������� �� ��������, ������������ �������� �������� - ���� 
        /// mode_stub.
        step* operator[]( int idx );

		/// @brief ������� � ��������� ����.
        ///
        /// @param new_step - ���, � �������� ���� �������.
		void to_step( unsigned int new_step );

        /// @brief ������� � ��������� ����.
        ///
        /// @param new_step - ���, � �������� ���� �������.
        /// @param cooper_time - ����� ������������ �����.
        void to_step( unsigned int new_step, unsigned int cooper_time );
		}
//-----------------------------------------------------------------------------
/// @brief �������� ���������� �� �����������, ������� ������ � ��� (�����������/
/// �����������).
///
/// � ������ ����� ���� �������� (�����������) ������ ���� ���.
class step
    {    
    public:
        /// @brief ��������� �������� ����� �������� ��������������.
        ///
        /// @param index - ������ ��������.
        ///
        /// @return - �������� �������� � �������� ��������. ���� ������
        /// ������� �� ��������, ������������ �������� 0.
        action* operator[] ( int idx );

        enum ACTIONS
            {
            A_ON = 0,
            A_OFF,
            A_UPPER_SEATS_ON,
            A_LOWER_SEATS_ON = A_UPPER_SEATS_ON,

            A_REQUIRED_FB,            
            A_DO_DI,
            A_WASH,
            };
    };
//-----------------------------------------------------------------------------
/// @brief �������� ��� ������������ (���������, ���������� � �.�.).
class action
    {
    public:
        /// @brief ���������� ���������� � ��������.
        ///
        /// @param [in] dev ����������.        
        /// @param [in] group ������, ���� ����������� ����������.      
        virtual void add_dev( device *dev, unsigned int group );

        /// @brief ���������� ���������� � ��������.
        ///
        /// @param [in] dev ����������.
        /// @param [in] group �������������� ��������.
        /// @param [in] subgroup �������������� ��������.
        virtual void add_dev( device *dev, unsigned int group, 
            unsigned int subgroup );
    };
//-----------------------------------------------------------------------------
///@brief ��������� ��������� ���������.
///
///@return �������� ��������� �������.
device_manager* G_DEVICE_MANAGER();

///@brief ��������� ���������������� ������� �� ������.
///
///@return ��������������� ������.
tech_object* G_TECH_OBJECTS( unsigned int idx );
//-----------------------------------------------------------------------------
/// @brief �������� ���������.
///
/// �������� ���������� ��� ���� ����������� �������.
class device_manager
    {
    public:
        /// @brief ���������� ������ ������� � �������.
        void print() const;

        // @brief ��������� ����� ���������.
        //
        // ���������� �� Lua.
        wago_device* add_wago_device( int dev_type, int dev_sub_type,
            const char *dev_name, char * descr );

        /// @brief ��������� ���������� �� ��� ������.
        device* get_device( int dev_type,
            const char *dev_name );
    };
//-----------------------------------------------------------------------------
/// @brief ���������� �� ������ ������� �����/������ WAGO.
///
/// � ����� ������ � ���������� ����� ���� ���� ��� ��������� �������
/// �����/������ (���������� ��� ����������).
class wago_device
    {
    public:
        // Lua.
        void init( int DO_count, int DI_count,
            int AO_count, int AI_count );

        void init_channel( int type, int ch_inex, int node, int offset );
    };
//-----------------------------------------------------------------------------
/// @brief ������ ��������.
class timer_manager
    {
    public:
        /// @brief ���������� ��������� ������� �� �������.
        ///
        /// @param index - ������ �������.
        ///
        /// @return - ������ � ������ ��������, �������� - � ������ ������ ��
        /// ��������.
        timer* operator[] ( unsigned int index );
    };
//-----------------------------------------------------------------------------
/// @brief ������.
class timer
    {
    public:
        /// @brief ��������� �������.
        enum STATE
            {
            S_STOP = 0, ///< �� ��������.
            S_WORK,     ///< ��������.
            S_PAUSE,    ///< �����.
            };

        /// @brief ������ �������.
        void start();

        /// @brief ����� �������.
        void reset();

        /// @brief ����� �������.
        void pause();

        /// @brief �������� ������ ������� �������.
        ///
        /// @return true  - ����� �����.
        /// @return false - ����� �� �����.
        bool is_time_up() const;

        /// @brief ��������� ������� ������ �������.
        ///
        /// @return - ����� ������ �������.
        unsigned long  get_work_time() const;

        /// @brief ��������� ������� �������.
        ///
        /// @param new_countdown_time - �������.
        void set_countdown_time( unsigned long new_countdown_time );

        /// @brief ��������� ������� �������.
        ///
        /// @return - ������� �������.
        unsigned long  get_countdown_time() const;

        /// @brief ��������� ��������� �������.
        ///
        /// @return - ��������� �������.
        STATE get_state() const;

        timer();
    };
//-----------------------------------------------------------------------------
/// @brief ������ � ������������ ����������� ���� "������� �����".
///
/// ��������� ����������� � ����������������� ������.
class saved_params_float
    {
    public:
        /// @brief ���������� �������� @a value � �������� � ������� @a idx
        /// � ����������������� ������.
        ///
        /// @param idx   - ������ ���������.
        /// @param value - �������� ���������.
        int save( unsigned int idx, float value );

        /// @brief ���������� ���� ���������� � ����������������� ������.
        int save_all();

        /// @brief ��������� ��������� �� ��� ������.
        ///
        /// ��� ������������ ��������� ������ �������� (�������� par[ 5 ] = 6),
        /// �������� ����������� ������ � ����������� ������ (�� ��������
        ///	�������� ����� ������������ �����������).
        ///
        /// @param idx   - ������ ���������.
        ///
        /// @return - �������� c �������� ��������.
        float& operator[] ( unsigned int idx );

        /// @brief ��������� ���� ���������� � ������� �������� � ����������
        /// � ����������������� ������.
        void reset_to_0();
    };
//-----------------------------------------------------------------------------
/// @brief ������ � ������������ ����������� ���� "����������� ����� �����".
///
/// ��������� ����������� � ����������������� ������.
class saved_params_u_int_4
    {
    public:
        /// @brief ���������� �������� @a value � �������� � ������� @a idx
        /// � ����������������� ������.
        int save( unsigned int idx, unsigned int value );

        /// @brief ���������� ���� ���������� � ����������������� ������.
        int save_all();

        /// @brief ��������� ��������� �� ��� ������.
        ///
        /// ��� ������������ ��������� ������ �������� (�������� par[ 5 ] = 6),
        /// �������� ����������� ������ � ����������� ������ (�� ��������
        ///	�������� ����� ������������ �����������).
        ///
        /// @param idx   - ������ ���������.
        ///
        /// @return - �������� c �������� ��������.
        unsigned int& operator[] ( unsigned int idx );

        /// @brief ��������� ���� ���������� � ������� �������� � ����������
        /// � ����������������� ������.
        void reset_to_0();
    };
//-----------------------------------------------------------------------------
/// @brief ������ � �������� ����������� ���� "����������� ����� �����".
///
/// ��������� ����������� � ����������� ������.
class run_time_params_u_int_4
    {
    public:
        /// @brief ��������� ��������� �� ��� ������.
        ///
        /// @param idx - ������ ���������.
        ///
        /// @return - �������� c �������� ��������.
        unsigned int& operator[] ( unsigned int idx );

        /// @brief ��������� ���� ���������� � ������� ��������.
        void reset_to_0();
    };
//-----------------------------------------------------------------------------
/// @brief ������ � �������� ����������� ���� "������� �����".
///
/// ��������� ����������� � ����������� ������.
class run_time_params_float
    {
    public:
        /// @brief ��������� ��������� �� ��� ������.
        ///
        /// @param idx - ������ ���������.
        ///
        /// @return - �������� c �������� ��������.
        float& operator[] ( unsigned int idx );

        /// @brief ��������� ���� ���������� � ������� ��������.
        void reset_to_0();
    };
//-----------------------------------------------------------------------------
/// @brief ������ � �������� �����/������ Wago.
///
/// ���������� ������ � ������ ��������� ������� �����/������ Wago.
class wago_manager
    {
    public:
        /// @brief ��������� ����� �������.
        ///
        /// ���������� �� Lua.
        void init( int nodes_count );

        /// @brief ������������� ������ Wago.
        ///
        /// ���������� �� Lua.
        void add_node(  unsigned int index, int ntype, int address,
            char* IP_address, char *name, int DO_cnt, int DI_cnt, int AO_cnt, int AO_size,
            int AI_cnt, int AI_size );

        /// @brief ������������� ���������� ������ ����������� ������.
        ///
        /// ���������� �� Lua.
        void init_node_AO( unsigned int node_index, unsigned int AO_index,
            unsigned int type, unsigned int offset );

        /// @brief ������������� ���������� ������ ����������� �����.
        ///
        /// ���������� �� Lua.
        void init_node_AI( unsigned int node_index, unsigned int AI_index,
            unsigned int type, unsigned int offset );

    };
//-----------------------------------------------------------------------------
///@brief ��������� ��������� Wago.
///
///@return �������� ��������� �������.
wago_manager* G_WAGO_MANAGER();
//-----------------------------------------------------------------------------
///@brief ���-���������.
///
///
class PID
    {
    public:

        ///@brief �������� ����������� ���������.
        enum PARAM
        {
        P_k = 1,               ///< �������� k.
        P_Ti,                  ///< �������� Ti.
        P_Td,                  ///< �������� Td.
        P_dt,                  ///< �������� �������
        P_max,                 ///< �ax �������� ������� ��������.
        P_min,                 ///< �in �������� ������� ��������.
        P_acceleration_time,   ///< ����� ������ �� ����� �������������.
        P_is_manual_mode,      ///< ������ �����.
        P_U_manual,            ///< �������� ������ �������� ��������� �������.

        P_k2,                  ///< �������� k2.
        P_Ti2,                 ///< �������� Ti2.
        P_Td2,                 ///< �������� Td2.
        };

        ///@brief ������� ���������.
        enum WORK_PARAM
        {
        WP_Z = 1,  ///< ��������� ��������.
        WP_U,      ///< ����� ���.
        };

        /// @param n - �����.
        PID( int n );

        ///@brief ��������� ����������.
        void  on( char is_down_to_inaccel_mode = 0 );

        ///@brief ���������� ����������.
        void  off();

        ///@brief ��������� ���������� �������� ������ �� ������ ��������
        /// �������� �����.
        float eval( float current_value, int delta_sign = 1 );

        ///@brief ������� ����� ������� ���.
        void  set( float new_z );

        /// @brief ��������� ������� ���.
        float get_assignment();

        /// @brief ������ ������������ ���������.
        void init_param( PARAM par_n, float val );

        /// @brief ������ �������� ���������.
        void init_work_param( WORK_PARAM par_n, float val );

        /// @brief ������ ����������� ���������� � EEPROM.
        void save_param();

        /// @brief ������� ��� ������������� �������������� P_k2, P_Ti2, P_Td2.
        void  set_used_par ( int par_n );

        /// @brief ���������� ������ ������� � �������.
        void print();

		int set_cmd( const char *prop, unsigned int idx, double val );
    };
//-----------------------------------------------------------------------------
class PAC_info: public i_Lua_save_device
    {
    public:
        int set_cmd( const char *prop, unsigned int idx, double val );
	};


PAC_info* G_PAC_INFO();
//-----------------------------------------------------------------------------
/// @brief ��������� �������� ������� � ��������.
///
/// @return ������� �����.
/// @warning ����� ������������ � �������� � 01.01.1970, � 2038 ���������� 
/// ������������.
unsigned long get_sec();
//-----------------------------------------------------------------------------
/// @brief ��������� ������� � �������������.
///
/// ��� ��� �� 4 ��� ���������� ������������ � ������ ������������ � 0, �� ���
/// ���������� �������� ������� ������������ @ref get_delta_millisec.
///
/// @return ����� � ������� ������� ��������� � �������������.
unsigned long get_millisec();
//-----------------------------------------------------------------------------
/// @brief ��������� �������� ������� � �������������.
///
/// @param time1     - ��������� �����.
/// @return �������� ������� � �������������.
unsigned long get_delta_millisec( unsigned long time1 );
//-----------------------------------------------------------------------------
/// @brief �������� �������� �����.
///
/// @param ms - ����� ��������, ��.
void sleep_ms( unsigned long ms );

/// @brief ����� ��� ������ ������� �������
class cipline_tech_object: public tech_object
	{
	public:
		cipline_tech_object( const char* name, unsigned int number, unsigned int type, const char* name_Lua,
			unsigned int states_count,
			unsigned int timers_count,
			unsigned int par_float_count, unsigned int runtime_par_float_count,
			unsigned int par_uint_count, unsigned int runtime_par_uint_count );

		int msa_number; //����� �������
		saved_params_float      par_float;   ///< ����������� ���-��, ��� float.
		run_time_params_float   rt_par_float;///< ������� ���������, ��� float.
		float get_station_par(int parno);
		void set_station_par(int parno, float newval);
		
		int set_cmd( const char *prop, unsigned int idx, const char* val );
		int set_cmd( const char *prop, unsigned int idx, double val );

		PID* PIDFlow;
		PID* PIDPump;
		void initline();
		int evaluate();
	};
//---------------------------------------------------------------------------
rm_manager* G_RM_MANAGER();
//---------------------------------------------------------------------------
class rm_manager
    {
    public:      
        /// <summary>
        /// ���������� ���������� PAC ��� ����������.
        /// </summary>
        /// <param name="name">��� ���������� PAC.</param>
        /// <param name="IP_address">IP-����� ���������� PAC.</param>
        /// <param name="remote_PAC_id">����������������� �����
        /// ���������� PAC.</param>
        ///
        /// <returns></returns>
        void add_rm_cmmctr( char* name, char* IP_address, 
            int remote_PAC_id );
    };   
//---------------------------------------------------------------------------

class modbus_client 
	{
	protected:

	public:
		modbus_client(unsigned int id, char* ip, unsigned int port, unsigned long exchangetimeout);
		//���������� ������� ��������� modbus
		int read_discrete_inputs(unsigned int start_address, unsigned int quantity);
		int read_coils(unsigned int start_address, unsigned int quantity);
		int read_holding_registers(unsigned int address, unsigned int quantity);
		int read_input_registers(unsigned int address, unsigned int quantity);
		int write_coil(unsigned int address, unsigned char value);
		int force_multiply_coils(unsigned int address, unsigned int quantity);
		int write_multiply_registers(unsigned int address, unsigned int quantity);
		//������� ��� ������ � ������� �� Lua
		void zero_output_buff();
		void set_int2(unsigned int address, short value);
		short get_int2(unsigned int address);
		void set_int4(unsigned int address, int value);
		int get_int4(unsigned int address);
		void set_float(unsigned int address, float value);
		float get_float(unsigned int address);
		void set_bit(unsigned int address, int value);
		int get_bit(unsigned int address);
	};