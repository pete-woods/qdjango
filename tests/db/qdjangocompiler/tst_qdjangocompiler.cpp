/*
 * Copyright (C) 2010-2014 Jeremy Lainé
 * Contact: https://github.com/jlaine/qdjango
 *
 * This file is part of the QDjango Library.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 */

#include <QSqlDriver>

#include "QDjango.h"
#include "QDjangoQuerySet.h"
#include "QDjangoWhere.h"

#include "util.h"

static QStringList normalizeNames(const QSqlDatabase &db, const QStringList &rawNames)
{
    QStringList normalizedNames;
    foreach (const QString &rawName, rawNames) {
        normalizedNames << normalizeSql(db, rawName);
    }
    return normalizedNames;
}

class Item : public QDjangoModel
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName)

public:
    Item(QObject *parent = 0);

    QString name() const;
    void setName(const QString &name);

private:
    QString m_name;
};

class Owner : public QDjangoModel
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(Item* item1 READ item1 WRITE setItem1)
    Q_PROPERTY(Item* item2 READ item2 WRITE setItem2)

public:
    Owner(QObject *parent = 0);

    QString name() const;
    void setName(const QString &name);

    Item *item1() const;
    void setItem1(Item *item1);

    Item *item2() const;
    void setItem2(Item *item2);

private:
    QString m_name;
};

class OwnerWithNullableItem : public QDjangoModel
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(Item* item1 READ item1 WRITE setItem1)
    Q_PROPERTY(Item* item2 READ item2 WRITE setItem2)
    Q_CLASSINFO("item2", "null=true")

public:
    OwnerWithNullableItem(QObject *parent = 0);

    QString name() const;
    void setName(const QString &name);

    Item *item1() const;
    void setItem1(Item *item1);

    Item *item2() const;
    void setItem2(Item *item2);

private:
    QString m_name;
};

class Top : public QDjangoModel
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(Owner *owner READ owner WRITE setOwner)

public:
    Top(QObject *parent = 0);

    QString name() const;
    void setName(const QString &name);

    Owner *owner() const;
    void setOwner(Owner *owner);

private:
    QString m_name;
};

class TopWithNullableItem : public QDjangoModel
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(OwnerWithNullableItem *owner READ owner WRITE setOwner)

public:
    TopWithNullableItem(QObject *parent = 0);

    QString name() const;
    void setName(const QString &name);

    OwnerWithNullableItem *owner() const;
    void setOwner(OwnerWithNullableItem *owner);

private:
    QString m_name;
};

class tst_QDjangoCompiler : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void fieldNames_data();
    void fieldNames();
    void resolve_data();
    void resolve();
};

Item::Item(QObject *parent)
    : QDjangoModel(parent)
{
}

QString Item::name() const
{
    return m_name;
}

void Item::setName(const QString &name)
{
    m_name = name;
}

Owner::Owner(QObject *parent)
    : QDjangoModel(parent)
{
    setForeignKey("item1", new Item(this));
    setForeignKey("item2", new Item(this));
}

QString Owner::name() const
{
    return m_name;
}

void Owner::setName(const QString &name)
{
    m_name = name;
}

Item* Owner::item1() const
{
    return qobject_cast<Item*>(foreignKey("item1"));
}

void Owner::setItem1(Item *item1)
{
    setForeignKey("item1", item1);
}

Item* Owner::item2() const
{
    return qobject_cast<Item*>(foreignKey("item2"));
}

void Owner::setItem2(Item *item2)
{
    setForeignKey("item2", item2);
}

OwnerWithNullableItem::OwnerWithNullableItem(QObject *parent)
    : QDjangoModel(parent)
{
    setForeignKey("item1", new Item(this));
    setForeignKey("item2", new Item(this));
}

QString OwnerWithNullableItem::name() const
{
    return m_name;
}

void OwnerWithNullableItem::setName(const QString &name)
{
    m_name = name;
}

Item* OwnerWithNullableItem::item1() const
{
    return qobject_cast<Item*>(foreignKey("item1"));
}

void OwnerWithNullableItem::setItem1(Item *item1)
{
    setForeignKey("item1", item1);
}

Item* OwnerWithNullableItem::item2() const
{
    return qobject_cast<Item*>(foreignKey("item2"));
}

void OwnerWithNullableItem::setItem2(Item *item2)
{
    setForeignKey("item2", item2);
}

Top::Top(QObject *parent)
    : QDjangoModel(parent)
{
}

QString Top::name() const
{
    return m_name;
}

void Top::setName(const QString &name)
{
    m_name = name;
}

Owner* Top::owner() const
{
    return qobject_cast<Owner*>(foreignKey("owner"));
}

void Top::setOwner(Owner *owner)
{
    setForeignKey("owner", owner);
}

TopWithNullableItem::TopWithNullableItem(QObject *parent)
    : QDjangoModel(parent)
{
}

QString TopWithNullableItem::name() const
{
    return m_name;
}

void TopWithNullableItem::setName(const QString &name)
{
    m_name = name;
}

OwnerWithNullableItem* TopWithNullableItem::owner() const
{
    return qobject_cast<OwnerWithNullableItem*>(foreignKey("owner"));
}

void TopWithNullableItem::setOwner(OwnerWithNullableItem *owner)
{
    setForeignKey("owner", owner);
}

void tst_QDjangoCompiler::initTestCase()
{
    QVERIFY(initialiseDatabase());
    QDjango::registerModel<Item>();
    QDjango::registerModel<Owner>();
    QDjango::registerModel<OwnerWithNullableItem>();
    QDjango::registerModel<Top>();
    QDjango::registerModel<TopWithNullableItem>();
}

void tst_QDjangoCompiler::fieldNames_data()
{
    QSqlDatabase db = QDjango::database();

    QTest::addColumn<QByteArray>("modelName");
    QTest::addColumn<bool>("recursive");
    QTest::addColumn<QStringList>("fieldNames");
    QTest::addColumn<QStringList>("orderBy");
    QTest::addColumn<QString>("orderSql");
    QTest::addColumn<QString>("fromSql");

    QTest::newRow("non recursive") << QByteArray("Owner") << false << (QStringList()
        << "\"owner\".\"id\""
        << "\"owner\".\"name\""
        << "\"owner\".\"item1_id\""
        << "\"owner\".\"item2_id\"")
    << QStringList()
    << ""
    << "\"owner\"";

    QTest::newRow("recurse one level") << QByteArray("Owner") << true << (QStringList()
        << "\"owner\".\"id\""
        << "\"owner\".\"name\""
        << "\"owner\".\"item1_id\""
        << "\"owner\".\"item2_id\""
        << "T0.\"id\""
        << "T0.\"name\""
        << "T1.\"id\""
        << "T1.\"name\"")
    << QStringList()
    << ""
    << "\"owner\""
       " INNER JOIN \"item\" T0 ON T0.\"id\" = \"owner\".\"item1_id\""
       " INNER JOIN \"item\" T1 ON T1.\"id\" = \"owner\".\"item2_id\"";

    QTest::newRow("recurse with nullable") << QByteArray("OwnerWithNullableItem") << true << (QStringList()
        << "\"ownerwithnullableitem\".\"id\""
        << "\"ownerwithnullableitem\".\"name\""
        << "\"ownerwithnullableitem\".\"item1_id\""
        << "\"ownerwithnullableitem\".\"item2_id\""
        << "T0.\"id\""
        << "T0.\"name\""
        << "T1.\"id\""
        << "T1.\"name\"")
    << QStringList()
    << ""
    << "\"ownerwithnullableitem\""
       " INNER JOIN \"item\" T0 ON T0.\"id\" = \"ownerwithnullableitem\".\"item1_id\""
       " LEFT OUTER JOIN \"item\" T1 ON T1.\"id\" = \"ownerwithnullableitem\".\"item2_id\"";

    QTest::newRow("recurse two levels") << QByteArray("Top") << true << (QStringList()
        << "\"top\".\"id\""
        << "\"top\".\"name\""
        << "\"top\".\"owner_id\""
        << "T0.\"id\""
        << "T0.\"name\""
        << "T0.\"item1_id\""
        << "T0.\"item2_id\""
        << "T1.\"id\""
        << "T1.\"name\""
        << "T2.\"id\""
        << "T2.\"name\"")
    << QStringList()
    << ""
    << "\"top\""
       " INNER JOIN \"owner\" T0 ON T0.\"id\" = \"top\".\"owner_id\""
       " INNER JOIN \"item\" T1 ON T1.\"id\" = T0.\"item1_id\""
       " INNER JOIN \"item\" T2 ON T2.\"id\" = T0.\"item2_id\"";

    QTest::newRow("recurse two levels with nullable item") << QByteArray("TopWithNullableItem") << true << (QStringList()
        << "\"topwithnullableitem\".\"id\""
        << "\"topwithnullableitem\".\"name\""
        << "\"topwithnullableitem\".\"owner_id\""
        << "T0.\"id\""
        << "T0.\"name\""
        << "T0.\"item1_id\""
        << "T0.\"item2_id\""
        << "T1.\"id\""
        << "T1.\"name\""
        << "T2.\"id\""
        << "T2.\"name\"")
    << QStringList()
    << ""
    << "\"topwithnullableitem\""
       " INNER JOIN \"ownerwithnullableitem\" T0 ON T0.\"id\" = \"topwithnullableitem\".\"owner_id\""
       " INNER JOIN \"item\" T1 ON T1.\"id\" = T0.\"item1_id\""
       " LEFT OUTER JOIN \"item\" T2 ON T2.\"id\" = T0.\"item2_id\"";

    // ordered
    QTest::newRow("order ascending") << QByteArray("Owner") << false
        << (QStringList()
            << "\"owner\".\"id\""
            << "\"owner\".\"name\""
            << "\"owner\".\"item1_id\""
            << "\"owner\".\"item2_id\"")
        << QStringList("name")
        << QString(" ORDER BY \"owner\".\"name\" ASC")
        << QString("\"owner\"");

    QTest::newRow("order ascending foreign") << QByteArray("Owner") << false
        << (QStringList()
            << "\"owner\".\"id\""
            << "\"owner\".\"name\""
            << "\"owner\".\"item1_id\""
            << "\"owner\".\"item2_id\"")
        << QStringList("item1__name")
        << QString(" ORDER BY T0.\"name\" ASC")
        << "\"owner\""
           " INNER JOIN \"item\" T0 ON T0.\"id\" = \"owner\".\"item1_id\"";

    QTest::newRow("order ascending foreign double") << QByteArray("Owner") << false
        << (QStringList()
            << "\"owner\".\"id\""
            << "\"owner\".\"name\""
            << "\"owner\".\"item1_id\""
            << "\"owner\".\"item2_id\"")
        << (QStringList() << "item1__name" << "item2__name")
        << QString(" ORDER BY T0.\"name\" ASC, T1.\"name\" ASC")
        << "\"owner\""
           " INNER JOIN \"item\" T0 ON T0.\"id\" = \"owner\".\"item1_id\""
           " INNER JOIN \"item\" T1 ON T1.\"id\" = \"owner\".\"item2_id\"";

    QTest::newRow("order descending") << QByteArray("Owner") << false
        << (QStringList()
            << "\"owner\".\"id\""
            << "\"owner\".\"name\""
            << "\"owner\".\"item1_id\""
            << "\"owner\".\"item2_id\"")
        << QStringList("-name")
        << QString(" ORDER BY \"owner\".\"name\" DESC")
        << QString("\"owner\"");
}

void tst_QDjangoCompiler::fieldNames()
{
    QFETCH(QByteArray, modelName);
    QFETCH(bool, recursive);
    QFETCH(QStringList, fieldNames);
    QFETCH(QStringList, orderBy);
    QFETCH(QString, orderSql);
    QFETCH(QString, fromSql);

    QSqlDatabase db = QDjango::database();

    QDjangoCompiler compiler(modelName, db);
    QCOMPARE(normalizeNames(db, compiler.fieldNames(recursive)), fieldNames);
    QCOMPARE(normalizeSql(db, compiler.orderLimitSql(orderBy, 0, 0)), orderSql);
    QCOMPARE(normalizeSql(db, compiler.fromSql()), fromSql);
}

void tst_QDjangoCompiler::resolve_data()
{
    QTest::addColumn<QByteArray>("modelName");
    QTest::addColumn<bool>("recursive");
    QTest::addColumn<QStringList>("fieldNames");
    QTest::addColumn<QDjangoWhere>("where");
    QTest::addColumn<QString>("whereSql");
    QTest::addColumn<QVariantList>("whereValues");
    QTest::addColumn<QString>("fromSql");

    QTest::newRow("local field") << QByteArray("Owner") << false
        << (QStringList()
            << "\"owner\".\"id\""
            << "\"owner\".\"name\""
            << "\"owner\".\"item1_id\""
            << "\"owner\".\"item2_id\"")
        << QDjangoWhere("name", QDjangoWhere::Equals, "foo")
        << "\"owner\".\"name\" = ?"
        << (QVariantList() << "foo")
        << "\"owner\"";

    QTest::newRow("foreign field") << QByteArray("Owner") << false
        << (QStringList()
            << "\"owner\".\"id\""
            << "\"owner\".\"name\""
            << "\"owner\".\"item1_id\""
            << "\"owner\".\"item2_id\"")
        << QDjangoWhere("item1__name", QDjangoWhere::Equals, "foo")
        << "T0.\"name\" = ?"
        << (QVariantList() << "foo")
        << "\"owner\""
           " INNER JOIN \"item\" T0 ON T0.\"id\" = \"owner\".\"item1_id\"";

    QTest::newRow("reverse field") << QByteArray("Owner") << false
        << (QStringList()
            << "\"owner\".\"id\""
            << "\"owner\".\"name\""
            << "\"owner\".\"item1_id\""
            << "\"owner\".\"item2_id\"")
        << QDjangoWhere("top__name", QDjangoWhere::Equals, "foo")
        << "T0.\"name\" = ?"
        << (QVariantList() << "foo")
        << "\"owner\""
           " INNER JOIN \"top\" T0 ON T0.\"owner_id\" = \"owner\".\"id\"";
    
    QTest::newRow("multiple fields") << QByteArray("Owner") << false
        << (QStringList()
            << "\"owner\".\"id\""
            << "\"owner\".\"name\""
            << "\"owner\".\"item1_id\""
            << "\"owner\".\"item2_id\"")
        << (QDjangoWhere("item1__name", QDjangoWhere::Equals, "foo")
           && QDjangoWhere("item2__name", QDjangoWhere::Equals, "bar"))
        << "T0.\"name\" = ? AND T1.\"name\" = ?"
        << (QVariantList() << "foo" << "bar")
        << "\"owner\""
           " INNER JOIN \"item\" T0 ON T0.\"id\" = \"owner\".\"item1_id\""
           " INNER JOIN \"item\" T1 ON T1.\"id\" = \"owner\".\"item2_id\"";
}

void tst_QDjangoCompiler::resolve()
{
    QSqlDatabase db = QDjango::database();

    QFETCH(QByteArray, modelName);
    QFETCH(bool, recursive);
    QFETCH(QStringList, fieldNames);
    QFETCH(QDjangoWhere, where);
    QFETCH(QString, whereSql);
    QFETCH(QVariantList, whereValues);
    QFETCH(QString, fromSql);

    QDjangoCompiler compiler("Owner", db);
    compiler.resolve(where);
    CHECKWHERE(where, whereSql, whereValues);
    QCOMPARE(normalizeNames(db, compiler.fieldNames(recursive)), fieldNames);
    QCOMPARE(normalizeSql(db, compiler.fromSql()), fromSql);
}

Q_DECLARE_METATYPE(QDjangoWhere)
QTEST_MAIN(tst_QDjangoCompiler)
#include "tst_qdjangocompiler.moc"
