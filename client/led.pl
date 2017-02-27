#!/usr/bin/perl

package LED;

use strict;
use warnings;

use Getopt::Long;
use Pod::Usage;
use Data::Dumper;
use LED::BlueTooth;
use LED::Image;

sub main {
	my %opts;

	GetOptions(
		'help|h'	=> \$opts{show_help},
		'image=s'	=> \$opts{image},
		'width=i'	=> \$opts{width},
		'height=i'	=> \$opts{height},
		'serial-port=s'	=> \$opts{serial_port},
	) or pod2usage(2);

	my @required = qw( image );
	for (@required) {
		unless ($opts{$_}) {
			warn "Option $_ is required\n";
			$opts{show_help} = 1;
		}
	}

	return pod2usage(1)
		if $opts{show_help};

	my $img = LED::Image->new($opts{image});

	my $connector;
	if (defined $opts{serial_port}) {
		warn "TEST";
		$connector = LED::BlueTooth->connect_serial($opts{serial_port})
			or die "can't connect to serial\n";

		warn "TEST";
	}

	my $data = $img->convert(sub{ my ($g, $r, $b) = @_; return pack 'WWW', $g, $r, $b; });
	unless ($connector) {
		warn "Data is $data\n";
	} else {
		warn "Sleeping";
		$connector->write($data);
		warn "Sleeping";
		sleep 5;
		warn $connector->read();
		warn "Sleeping";
		sleep 5;
		$connector->close();
	}
}

main;

__END__

=head1 NAME

led.pl - simple images convertor for LED-T project.

=head1 SYNOPSIS

led.pl [options] image.bmp

=head1 OPTIONS

=over 8

=item B<--image>

Image to be transfered on controller.
Option is required.

=item B<--width>

Matrix width. Image width will be used if option was not set.

=item B<--height>

Matrix height. Image height will be used if option was not set.

=item B<--serial-port>

Path to the serial device to send data through.

=item B<--help>

Print help message and exit.

