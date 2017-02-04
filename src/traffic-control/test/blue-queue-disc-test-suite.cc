/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2016 NITK Surathkal
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Authors: Vivek Jain <jain.vivek.anand@gmail.com>
 *          Sandeep Singh <hisandeepsingh@hotmail.com>
 *          Mohit P. Tahiliani <tahiliani@nitk.edu.in>
 */

#include "ns3/test.h"
#include "ns3/blue-queue-disc.h"
#include "ns3/drop-tail-queue.h"
#include "ns3/uinteger.h"
#include "ns3/string.h"
#include "ns3/double.h"
#include "ns3/log.h"
#include "ns3/simulator.h"

using namespace ns3;

class BlueQueueDiscTestItem : public QueueDiscItem
{
public:
  BlueQueueDiscTestItem (Ptr<Packet> p, const Address & addr, uint16_t protocol);
  virtual ~BlueQueueDiscTestItem ();
  virtual void AddHeader (void);

private:
  BlueQueueDiscTestItem ();
  BlueQueueDiscTestItem (const BlueQueueDiscTestItem &);
  BlueQueueDiscTestItem &operator = (const BlueQueueDiscTestItem &);
};

BlueQueueDiscTestItem::BlueQueueDiscTestItem (Ptr<Packet> p, const Address & addr, uint16_t protocol)
  : QueueDiscItem (p, addr, protocol)
{
}

BlueQueueDiscTestItem::~BlueQueueDiscTestItem ()
{
}

void
BlueQueueDiscTestItem::AddHeader (void)
{
}

class BlueQueueDiscTestCase : public TestCase
{
public:
  BlueQueueDiscTestCase ();
  virtual void DoRun (void);
private:
  void Enqueue (Ptr<BlueQueueDisc> queue, uint32_t size, uint32_t nPkt);
  void EnqueueWithDelay (Ptr<BlueQueueDisc> queue, uint32_t size, uint32_t nPkt);
  void RunBlueTest (StringValue mode);
};

BlueQueueDiscTestCase::BlueQueueDiscTestCase ()
  : TestCase ("Sanity check on the blue queue disc implementation")
{
}

void
BlueQueueDiscTestCase::RunBlueTest (StringValue mode)
{
  uint32_t pktSize = 0;
  // 1 for packets; pktSize for bytes
  uint32_t modeSize = 1;
  uint32_t qLimit = 8;
  Ptr<BlueQueueDisc> queue = CreateObject<BlueQueueDisc> ();

  // test 1: simple enqueue/dequeue with no drops
  NS_TEST_EXPECT_MSG_EQ (queue->SetAttributeFailSafe ("Mode", mode), true,
                         "Verify that we can actually set the attribute Mode");
  NS_TEST_EXPECT_MSG_EQ (queue->SetAttributeFailSafe ("QueueLimit", UintegerValue (qLimit)), true,
                         "Verify that we can actually set the attribute QueueLimit");

  Address dest;

  if (queue->GetMode () == Queue::QUEUE_MODE_BYTES)
    {
      pktSize = 1000;
      modeSize = pktSize;
      queue->SetQueueLimit (qLimit * modeSize);
    }

  Ptr<Packet> p1, p2, p3, p4, p5, p6, p7, p8;
  p1 = Create<Packet> (pktSize);
  p2 = Create<Packet> (pktSize);
  p3 = Create<Packet> (pktSize);
  p4 = Create<Packet> (pktSize);
  p5 = Create<Packet> (pktSize);
  p6 = Create<Packet> (pktSize);
  p7 = Create<Packet> (pktSize);
  p8 = Create<Packet> (pktSize);

  queue->Initialize ();
  NS_TEST_EXPECT_MSG_EQ (queue->GetQueueSize (), 0 * modeSize, "There should be no packets in there");
  queue->Enqueue (Create<BlueQueueDiscTestItem> (p1, dest, 0));
  NS_TEST_EXPECT_MSG_EQ (queue->GetQueueSize (), 1 * modeSize, "There should be one packet in there");
  queue->Enqueue (Create<BlueQueueDiscTestItem> (p2, dest, 0));
  NS_TEST_EXPECT_MSG_EQ (queue->GetQueueSize (), 2 * modeSize, "There should be two packets in there");
  queue->Enqueue (Create<BlueQueueDiscTestItem> (p3, dest, 0));
  queue->Enqueue (Create<BlueQueueDiscTestItem> (p4, dest, 0));
  queue->Enqueue (Create<BlueQueueDiscTestItem> (p5, dest, 0));
  queue->Enqueue (Create<BlueQueueDiscTestItem> (p6, dest, 0));
  queue->Enqueue (Create<BlueQueueDiscTestItem> (p7, dest, 0));
  queue->Enqueue (Create<BlueQueueDiscTestItem> (p8, dest, 0));
  NS_TEST_EXPECT_MSG_EQ (queue->GetQueueSize (), 8 * modeSize, "There should be eight packets in there");

  Ptr<QueueDiscItem> item;

  item = queue->Dequeue ();
  NS_TEST_EXPECT_MSG_EQ ((item != 0), true, "I want to remove the first packet");
  NS_TEST_EXPECT_MSG_EQ (queue->GetQueueSize (), 7 * modeSize, "There should be seven packets in there");
  NS_TEST_EXPECT_MSG_EQ (item->GetPacket ()->GetUid (), p1->GetUid (), "was this the first packet ?");

  item = queue->Dequeue ();
  NS_TEST_EXPECT_MSG_EQ ((item != 0), true, "I want to remove the second packet");
  NS_TEST_EXPECT_MSG_EQ (queue->GetQueueSize (), 6 * modeSize, "There should be six packet in there");
  NS_TEST_EXPECT_MSG_EQ (item->GetPacket ()->GetUid (), p2->GetUid (), "Was this the second packet ?");

  item = queue->Dequeue ();
  NS_TEST_EXPECT_MSG_EQ ((item != 0), true, "I want to remove the third packet");
  NS_TEST_EXPECT_MSG_EQ (queue->GetQueueSize (), 5 * modeSize, "There should be five packets in there");
  NS_TEST_EXPECT_MSG_EQ (item->GetPacket ()->GetUid (), p3->GetUid (), "Was this the third packet ?");

  item = queue->Dequeue ();
  item = queue->Dequeue ();
  item = queue->Dequeue ();
  item = queue->Dequeue ();
  item = queue->Dequeue ();

  item = queue->Dequeue ();
  NS_TEST_EXPECT_MSG_EQ ((item == 0), true, "There are really no packets in there");

  // save number of drops from tests
  struct d
  {
    uint32_t test2;
    uint32_t test3;
    uint32_t test4;
  } drop;


  // test 2: default values for BLUE parameters
  queue = CreateObject<BlueQueueDisc> ();
  double Pmark = 0.2;
  double increment = 0.0025;
  double decrement = 0.00025;
  qLimit = 300 * modeSize;
  NS_TEST_EXPECT_MSG_EQ (queue->SetAttributeFailSafe ("Mode", mode), true,
                         "Verify that we can actually set the attribute Mode");
  NS_TEST_EXPECT_MSG_EQ (queue->SetAttributeFailSafe ("QueueLimit", UintegerValue (qLimit)), true,
                         "Verify that we can actually set the attribute QueueLimit");
  NS_TEST_EXPECT_MSG_EQ (queue->SetAttributeFailSafe ("PMark", DoubleValue (Pmark)), true,
                         "Verify that we can actually set the attribute PMark");
  NS_TEST_EXPECT_MSG_EQ (queue->SetAttributeFailSafe ("Increment", DoubleValue (increment)), true,
                         "Verify that we can actually set the attribute Increment");
  NS_TEST_EXPECT_MSG_EQ (queue->SetAttributeFailSafe ("Decrement", DoubleValue (decrement)), true,
                         "Verify that we can actually set the attribute Decrement");
  NS_TEST_EXPECT_MSG_EQ (queue->SetAttributeFailSafe ("FreezeTime", TimeValue (Seconds (0.1))), true,
                         "Verify that we can actually set the attribute FreezeTime");
  queue->Initialize ();
  EnqueueWithDelay (queue, pktSize, 300);
  Simulator::Run ();
  BlueQueueDisc::Stats st = StaticCast<BlueQueueDisc> (queue)->GetStats ();
  drop.test2 = st.unforcedDrop;
  NS_TEST_EXPECT_MSG_NE (drop.test2, 0, "There should be some unforced drops");


  // test 3: higher increment value for Pmark
  queue = CreateObject<BlueQueueDisc> ();
  increment = 0.25;
  NS_TEST_EXPECT_MSG_EQ (queue->SetAttributeFailSafe ("Mode", mode), true,
                         "Verify that we can actually set the attribute Mode");
  NS_TEST_EXPECT_MSG_EQ (queue->SetAttributeFailSafe ("QueueLimit", UintegerValue (qLimit)), true,
                         "Verify that we can actually set the attribute QueueLimit");
  NS_TEST_EXPECT_MSG_EQ (queue->SetAttributeFailSafe ("PMark", DoubleValue (Pmark)), true,
                         "Verify that we can actually set the attribute PMark");
  NS_TEST_EXPECT_MSG_EQ (queue->SetAttributeFailSafe ("Increment", DoubleValue (increment)), true,
                         "Verify that we can actually set the attribute Increment");
  NS_TEST_EXPECT_MSG_EQ (queue->SetAttributeFailSafe ("Decrement", DoubleValue (decrement)), true,
                         "Verify that we can actually set the attribute Decrement");
  NS_TEST_EXPECT_MSG_EQ (queue->SetAttributeFailSafe ("FreezeTime", TimeValue (Seconds (0.1))), true,
                         "Verify that we can actually set the attribute FreezeTime");
  queue->Initialize ();
  EnqueueWithDelay (queue, pktSize, 300);
  Simulator::Run ();
  st = StaticCast<BlueQueueDisc> (queue)->GetStats ();
  drop.test3 = st.unforcedDrop;
  NS_TEST_EXPECT_MSG_GT (drop.test3, drop.test2, "Test 3 should have more unforced drops than Test 2");


  // test 4: lesser time interval for updating Pmark
  queue = CreateObject<BlueQueueDisc> ();
  NS_TEST_EXPECT_MSG_EQ (queue->SetAttributeFailSafe ("Mode", mode), true,
                         "Verify that we can actually set the attribute Mode");
  NS_TEST_EXPECT_MSG_EQ (queue->SetAttributeFailSafe ("QueueLimit", UintegerValue (qLimit)), true,
                         "Verify that we can actually set the attribute QueueLimit");
  NS_TEST_EXPECT_MSG_EQ (queue->SetAttributeFailSafe ("PMark", DoubleValue (Pmark)), true,
                         "Verify that we can actually set the attribute PMark");
  NS_TEST_EXPECT_MSG_EQ (queue->SetAttributeFailSafe ("Increment", DoubleValue (increment)), true,
                         "Verify that we can actually set the attribute Increment");
  NS_TEST_EXPECT_MSG_EQ (queue->SetAttributeFailSafe ("Decrement", DoubleValue (decrement)), true,
                         "Verify that we can actually set the attribute Decrement");
  NS_TEST_EXPECT_MSG_EQ (queue->SetAttributeFailSafe ("FreezeTime", TimeValue (Seconds (0.01))), true,
                         "Verify that we can actually set the attribute FreezeTime");
  queue->Initialize ();
  EnqueueWithDelay (queue, pktSize, 300);
  Simulator::Run ();
  st = StaticCast<BlueQueueDisc> (queue)->GetStats ();
  drop.test4 = st.unforcedDrop;
  NS_TEST_EXPECT_MSG_GT (drop.test4, drop.test3, "Test 4 should have more unforced drops than Test 3");
}

void
BlueQueueDiscTestCase::Enqueue (Ptr<BlueQueueDisc> queue, uint32_t size, uint32_t nPkt)
{
  Address dest;
  for (uint32_t i = 0; i < nPkt; i++)
    {
      queue->Enqueue (Create<BlueQueueDiscTestItem> (Create<Packet> (size), dest, 0));
    }
}

void
BlueQueueDiscTestCase::EnqueueWithDelay (Ptr<BlueQueueDisc> queue, uint32_t size, uint32_t nPkt)
{
  Address dest;
  double delay = 0.001;
  for (uint32_t i = 0; i < nPkt; i++)
    {
      Simulator::Schedule (Time (Seconds ((i + 1) * delay)), &BlueQueueDiscTestCase::Enqueue, this, queue, size, 1);
    }
}

void
BlueQueueDiscTestCase::DoRun (void)
{
  RunBlueTest (StringValue ("QUEUE_MODE_PACKETS"));
  RunBlueTest (StringValue ("QUEUE_MODE_BYTES"));
  Simulator::Destroy ();
}

static class BlueQueueDiscTestSuite : public TestSuite
{
public:
  BlueQueueDiscTestSuite ()
    : TestSuite ("blue-queue-disc", UNIT)
  {
    AddTestCase (new BlueQueueDiscTestCase (), TestCase::QUICK);
  }
} g_blueQueueTestSuite;
