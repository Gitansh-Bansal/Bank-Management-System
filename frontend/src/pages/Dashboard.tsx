import React, { useState, useEffect } from 'react';
import { useNavigate } from 'react-router-dom';
import {
  Container,
  Box,
  Typography,
  Button,
  Grid,
  Card,
  CardContent,
  CardActions,
  Dialog,
  DialogTitle,
  DialogContent,
  DialogActions,
  TextField,
  MenuItem,
} from '@mui/material';
import { useAuth } from '../contexts/AuthContext';
import axios from 'axios';

interface Account {
  accountNumber: number;
  type: string;
  balance: number;
}

export default function Dashboard() {
  const [accounts, setAccounts] = useState<Account[]>([]);
  const [openDialog, setOpenDialog] = useState(false);
  const [accountType, setAccountType] = useState('');
  const [initialBalance, setInitialBalance] = useState('');
  const [accountPassword, setAccountPassword] = useState('');
  const navigate = useNavigate();
  const { user, logout } = useAuth();

  useEffect(() => {
    if (!user) {
      navigate('/login');
      return;
    }
    fetchAccounts();
  }, [user, navigate]);

  const fetchAccounts = async () => {
    try {
      const response = await axios.get(`http://localhost:3001/api/accounts?username=${user?.username}`);
      setAccounts(response.data.accounts || []);
    } catch (error) {
      console.error('Failed to fetch accounts:', error);
      setAccounts([]);
    }
  };

  const handleCreateAccount = async () => {
    try {
      await axios.post('http://localhost:3001/api/accounts', {
        username: user?.username,
        type: accountType,
        initialBalance: parseFloat(initialBalance),
        password: accountPassword,
      });
      setOpenDialog(false);
      setAccountType('');
      setInitialBalance('');
      setAccountPassword('');
      fetchAccounts();
    } catch (error) {
      console.error('Failed to create account:', error);
    }
  };

  const handleLogout = () => {
    logout();
    navigate('/login');
  };

  return (
    <Container>
      <Box sx={{ mt: 4, mb: 4 }}>
        <Box sx={{ display: 'flex', justifyContent: 'space-between', alignItems: 'center', mb: 4 }}>
          <Typography variant="h4" component="h1">
            Welcome, {user?.name}
          </Typography>
          <Box sx={{ display: 'flex', gap: 2 }}>
            <Button variant="outlined" color="primary" onClick={() => navigate('/profile')}>
              Profile
            </Button>
            <Button variant="outlined" color="primary" onClick={handleLogout}>
              Logout
            </Button>
          </Box>
        </Box>

        <Grid container spacing={3}>
          {accounts.map((account) => (
            <Grid item xs={12} sm={6} md={4} key={account.accountNumber}>
              <Card>
                <CardContent>
                  <Typography variant="h6" component="div">
                    Account #{account.accountNumber}
                  </Typography>
                  <Typography color="text.secondary">
                    Type: {account.type}
                  </Typography>
                  <Typography variant="h5" sx={{ mt: 2 }}>
                    ${account.balance.toFixed(2)}
                  </Typography>
                </CardContent>
                <CardActions>
                  <Button
                    size="small"
                    color="primary"
                    onClick={() => navigate(`/account/${account.accountNumber}`)}
                  >
                    View Details
                  </Button>
                </CardActions>
              </Card>
            </Grid>
          ))}
        </Grid>

        <Box sx={{ mt: 4, textAlign: 'center' }}>
          <Button
            variant="contained"
            color="primary"
            onClick={() => setOpenDialog(true)}
          >
            Create New Account
          </Button>
        </Box>

        <Dialog open={openDialog} onClose={() => setOpenDialog(false)}>
          <DialogTitle>Create New Account</DialogTitle>
          <DialogContent>
            <TextField
              select
              fullWidth
              label="Account Type"
              value={accountType}
              onChange={(e) => setAccountType(e.target.value)}
              margin="normal"
            >
              <MenuItem value="savings">Savings Account</MenuItem>
              <MenuItem value="current">Current Account</MenuItem>
              <MenuItem value="auditable">Auditable Savings</MenuItem>
            </TextField>
            <TextField
              fullWidth
              label="Initial Balance"
              type="number"
              value={initialBalance}
              onChange={(e) => setInitialBalance(e.target.value)}
              margin="normal"
            />
            <TextField
              fullWidth
              label="Account Password"
              type="password"
              value={accountPassword}
              onChange={(e) => setAccountPassword(e.target.value)}
              margin="normal"
            />
          </DialogContent>
          <DialogActions>
            <Button onClick={() => setOpenDialog(false)}>Cancel</Button>
            <Button onClick={handleCreateAccount} variant="contained" color="primary">
              Create
            </Button>
          </DialogActions>
        </Dialog>
      </Box>
    </Container>
  );
} 