#include <stdlib.h>

#include "temporal/beats.h"

#include "BeatTest.h"

CPPUNIT_TEST_SUITE_REGISTRATION(BeatsTest);

using namespace Temporal;

static const double delta = 1.5 / (double)Beats::PPQN;

void
BeatsTest::createTest()
{
	const Beats a(1, 2);
	CPPUNIT_ASSERT_EQUAL(int64_t(1), a.get_beats());
	CPPUNIT_ASSERT_EQUAL(2, a.get_ticks());
	CPPUNIT_ASSERT_DOUBLES_EQUAL(1 + 2 / (double)Beats::PPQN, DoubleableBeats(a).to_double(), delta);

	const Beats b = Beats::from_double (1.5);
	CPPUNIT_ASSERT_EQUAL(int64_t(1), b.get_beats());
	CPPUNIT_ASSERT_EQUAL(Beats::PPQN / 2, b.get_ticks());
	CPPUNIT_ASSERT_DOUBLES_EQUAL(1.5, DoubleableBeats (b).to_double(), delta);

	const Beats c = Beats::beats(6);
	CPPUNIT_ASSERT_EQUAL(int64_t(6), c.get_beats());
	CPPUNIT_ASSERT_EQUAL(0, c.get_ticks());

	const Beats d = Beats::ticks(7);
	CPPUNIT_ASSERT_EQUAL(int64_t(0), d.get_beats());
	CPPUNIT_ASSERT_EQUAL(7, d.get_ticks());

	Beats e(8, 9);
	e = d;
	CPPUNIT_ASSERT_EQUAL(d, e);


	// const Beats diff = n2 - n1;
	// CPPUNIT_ASSERT_EQUAL(-44, diff.get_beats());
	// CPPUNIT_ASSERT_EQUAL(44 / Beats::PPQN, diff.get_ticks());
	// CPPUNIT_ASSERT_DOUBLES_EQUAL(diff.to_double(), -44.44, delta);
}

void
BeatsTest::addTest()
{
	const Beats a(1, 2);
	const Beats b(3, 4);

	// Positive + positive
	const Beats c = a + b;
	CPPUNIT_ASSERT_EQUAL(int64_t(4), c.get_beats());
	CPPUNIT_ASSERT_EQUAL(6, c.get_ticks());

	const Beats n1 = Beats::from_double (-12.34);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(-12.34, DoubleableBeats (n1).to_double(), delta);

	const Beats n2 = Beats::from_double (-56.78);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(-56.78, DoubleableBeats (n2).to_double(), delta);

	// Positive + negative
	const Beats p1 = Beats::from_double (1.0);
	const Beats p_n = p1 + n1;
	CPPUNIT_ASSERT_EQUAL(-int64_t(11), p_n.get_beats());
	CPPUNIT_ASSERT_EQUAL((int32_t) rint (Beats::PPQN * -0.34), p_n.get_ticks());
	CPPUNIT_ASSERT_DOUBLES_EQUAL(-11.34, DoubleableBeats (p_n).to_double(), delta);

	// Negative + positive
	const Beats n_p = n1 + p1;
	CPPUNIT_ASSERT_EQUAL(-int64_t(11), n_p.get_beats());
	CPPUNIT_ASSERT_EQUAL((int32_t) rint (Beats::PPQN * -0.34), n_p.get_ticks());
	CPPUNIT_ASSERT_DOUBLES_EQUAL(-11.34, DoubleableBeats (n_p).to_double(), delta);

	// Negative + negative
	const Beats sum = n1 + n2;
	CPPUNIT_ASSERT_EQUAL(-int64_t(69), sum.get_beats());
	//CPPUNIT_ASSERT_EQUAL((int32_t)(Beats::PPQN * -0.12), n_p.get_ticks());
	CPPUNIT_ASSERT_DOUBLES_EQUAL(-69.12, DoubleableBeats (sum).to_double(), delta);
}

void
BeatsTest::subtractTest()
{
	const Beats a(1, 2);
	const Beats b(3, 4);

	// Positive - positive
	const Beats c = b - a;
	CPPUNIT_ASSERT_EQUAL(int64_t(2), c.get_beats());
	CPPUNIT_ASSERT_EQUAL(2, c.get_ticks());

	const Beats n1 = Beats::from_double (-12.34);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(-12.34, DoubleableBeats (n1).to_double(), delta);

	const Beats n2 = Beats::from_double (-56.78);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(-56.78, DoubleableBeats (n2).to_double(), delta);

	// Positive - negative
	const Beats p1= Beats::from_double (1.0);
	const Beats p_n = p1 - n1;
	CPPUNIT_ASSERT_EQUAL(int64_t(13), p_n.get_beats());
	CPPUNIT_ASSERT_EQUAL((int32_t) rint (Beats::PPQN * 0.34), p_n.get_ticks());
	CPPUNIT_ASSERT_DOUBLES_EQUAL(13.34, DoubleableBeats (p_n).to_double(), delta);

	// Negative - positive
	const Beats n_p = n1 - p1;
	CPPUNIT_ASSERT_EQUAL(-int64_t(13), n_p.get_beats());
	CPPUNIT_ASSERT_EQUAL((int32_t) rint (Beats::PPQN * -0.34), n_p.get_ticks());
	CPPUNIT_ASSERT_DOUBLES_EQUAL(-13.34, DoubleableBeats (n_p).to_double(), delta);

	// Negative - negative
	const Beats diff = n1 - n2;
	CPPUNIT_ASSERT_EQUAL(int64_t(44), diff.get_beats());
	CPPUNIT_ASSERT_EQUAL((int32_t)lrint(Beats::PPQN * 0.44), diff.get_ticks());
	CPPUNIT_ASSERT_DOUBLES_EQUAL(44.44, DoubleableBeats (diff).to_double(), delta);
}

void
BeatsTest::multiplyTest()
{
	CPPUNIT_ASSERT_DOUBLES_EQUAL(3.0, DoubleableBeats (Beats::from_double (1.5) * 2.0).to_double(), delta);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(-10.0, DoubleableBeats (Beats::from_double (5.0) * -2.0).to_double(), delta);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(-10.0, DoubleableBeats (Beats::from_double (-5.0) * 2.0).to_double(), delta);
}

void
BeatsTest::roundTest()
{
	Beats a(1, 1);

	// Round a up
	const Beats au = a.round_up_to_beat();
	CPPUNIT_ASSERT_EQUAL(au.get_beats(), int64_t(2));
	CPPUNIT_ASSERT_EQUAL(au.get_ticks(), 0);

	// Round a down
	const Beats ad = a.round_down_to_beat();
	CPPUNIT_ASSERT_EQUAL(ad.get_beats(), int64_t (1));
	CPPUNIT_ASSERT_EQUAL(ad.get_ticks(), 0);

	// Round result down again
	const Beats add = ad.round_down_to_beat();
	CPPUNIT_ASSERT_EQUAL(ad, add);

	// Round result up
	const Beats adu = ad.round_up_to_beat();
	CPPUNIT_ASSERT_EQUAL(ad, adu);

	// Snap to 1.5
	const Beats snapped = a.round_to_multiple (Beats::from_double (1.5));
	CPPUNIT_ASSERT_EQUAL(snapped.get_beats(), int64_t (1));
	CPPUNIT_ASSERT_EQUAL(snapped.get_ticks(), Beats::PPQN / 2);
}

void
BeatsTest::convertTest()
{
	const Beats a = Beats::ticks_at_rate(72000, 48000);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(int64_t (1), a.get_beats(), delta);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(Beats::PPQN / 2, a.get_ticks(), delta);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(1.5, DoubleableBeats(a).to_double(), delta);

	const Beats b = Beats::ticks_at_rate(8, 48000);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(int64_t (0), b.get_beats(), delta);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(Beats::PPQN * 8 / 48000, b.get_ticks(), delta);
	CPPUNIT_ASSERT_DOUBLES_EQUAL((8 / 48000.0), DoubleableBeats (b).to_double(), delta);

	CPPUNIT_ASSERT_EQUAL(int64_t(1.5 * Beats::PPQN), a.to_ticks());
	CPPUNIT_ASSERT_EQUAL(int64_t(1.5 * 192), DoubleableBeats (a).to_ticks(192));
}
