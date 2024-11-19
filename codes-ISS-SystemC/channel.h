#include <systemc.h>
#include "Utilities.h"

#ifndef _MY_channel_H_
#define _MY_channel_H_

class slave_port : virtual public sc_interface
{
public:
	virtual void put(sc_lv<16>) = 0;
	virtual void put(int) = 0;
	virtual void get(sc_lv<16>&) = 0;
	virtual void get(int&) = 0;
	virtual void get_info(operation&, sc_lv<16>&, burst&) = 0;
	virtual void get_info(transaction_info&) = 0;
	virtual void wait_for_call(sc_lv<16>, sc_lv<16>) = 0;
	virtual void wait_for_call(sc_lv<16>) = 0;
	virtual void wait_for_call() = 0;
	//virtual void request(int) = 0;
	//virtual void wait_for_ack() = 0;
};

class master_port : virtual public sc_interface
{
public:
	virtual void put(sc_lv<16>) = 0;
	virtual void put(int) = 0;
	virtual void get(sc_lv<16>&) = 0;
	virtual void get(int&) = 0;
	virtual void unlock() = 0;
	virtual void trylock(int, operation, sc_lv<16>, burst) = 0;
	virtual void trylock(int, transaction_info) = 0;
	//virtual void get_info(operation&, sc_lv<16>&, burst&) = 0;
	//virtual void wait_for_call(sc_lv<16>, sc_lv<16>) = 0;
	//virtual void wait_for_call(sc_lv<16>) = 0;
	//virtual void wait_for_call() = 0;
	//virtual void acknowledge() = 0;
	//virtual bool is_any_req() = 0;

	virtual void reset_started() = 0;
	virtual void reset_stopped() = 0;
};

class main_channel : public slave_port, public master_port
{
	operation bus_op;
	burst bus_burst_cnt;
	sc_lv<16> bus_address, bus_data;
	bool accesses[access_cnt];
	sc_event get_event, put_event, new_lock, new_unlock;

	sc_event get_ready, put_ready;
	bool get_entered, put_entered;

	int in_progress;
	bool arbitered;

	bool reset_occurred;

	//bool interrupt[10];
	//int inp_pri[10];
	//bool available_req;
	//bool ack;


public:
	main_channel() : in_progress(-1), arbitered(false), reset_occurred(false),
		get_entered(false), put_entered(false){};
	~main_channel() {};

	void put(sc_lv<16> data);
	void put(int data);
	void get(sc_lv<16>& data);
	void get(int& data);
	void get_info(operation &op, sc_lv<16> &address, burst &burst_cnt);
	void get_info(transaction_info &info);
	void wait_for_call(sc_lv<16> add_min, sc_lv<16> add_max);
	void wait_for_call(sc_lv<16> add);
	void wait_for_call();
	void unlock();
	void trylock(int priority, operation op, sc_lv<16> address, burst burst_cnt);
	void trylock(int priority, transaction_info info);
	void arbiter();

	void reset_started();
	void reset_stopped();
	void resseting();

	//void request(int priority);
	//void acknowledge();
	//void wait_for_ack();
	//bool is_any_req();
};

#endif